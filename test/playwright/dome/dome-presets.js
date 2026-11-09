// bench-auto: fixture dome.html
const { chromium } = require('playwright');

const TARGET_URL = process.env.TARGET_URL || 'http://127.0.0.1:4173/dome.html';
const HEADLESS = process.env.HEADLESS === 'true';

(async () => {
  const browser = await chromium.launch({ headless: HEADLESS, slowMo: HEADLESS ? 0 : 40 });
  const page = await browser.newPage({ viewport: { width: 1560, height: 920 } });

  await page.addInitScript(() => {
    class MockEventSource {
      constructor(url) {
        this.url = url;
        this.listeners = new Map();
        this.onerror = null;
        window.__mockEventSource = this;
      }

      addEventListener(type, handler) {
        if (!this.listeners.has(type)) this.listeners.set(type, []);
        this.listeners.get(type).push(handler);
      }

      close() {}

      emit(type, payload) {
        const handlers = this.listeners.get(type) || [];
        const data = typeof payload === 'string' ? payload : JSON.stringify(payload);
        handlers.forEach((handler) => handler({ data }));
      }
    }

    window.EventSource = MockEventSource;
    window.__emitMockStatus = (payload) => {
      if (!window.__mockEventSource) return false;
      window.__mockEventSource.emit('status', payload);
      return true;
    };
  });

  // Positive turns set to clockwise, so the word names a direction (#504):
  // the droid's own answer, held by this script rather than the shared
  // fixture, which ships Dome Rotation switched off.
  await page.route('**/api/config', (route) => (route.request().method() === 'GET'
    ? route.fulfill({ json: {
      system: { logLevel: 3 },
      components: { domeRotation: { enabled: true, label: 'DOME' } },
      domeRotation: { neutralUs: 1500, minPulseUs: 1000, maxPulseUs: 2000, speedLimitPct: 100, positiveTurn: 'cw' },
    } })
    : route.fulfill({ json: { ok: true } })));

  try {
    await page.goto(TARGET_URL, { waitUntil: 'networkidle' });
    await page.waitForSelector('.dome-live-track', { timeout: 10000 });
    await page.waitForFunction(() => typeof window.__emitMockStatus === 'function', { timeout: 10000 });

    // A frame the Live Reading will take: it refuses one missing any of the
    // fields it verifies (data/live_reading.js VERIFIABLE_STATUS_FIELDS) as
    // "not a reading", and a surface then keeps waiting. The three this
    // script varies ride on top of a clear, awake, radio-quiet droid.
    const VERIFIED_FRAME = {
      estop: false,
      sbusHwFailsafe: false,
      sbusSignalLost: false,
      webDriveExpired: false,
      webControlEnabled: false,
      sleepMode: false,
    };
    const emitStatus = async (payload) => {
      const frame = { ...VERIFIED_FRAME, ...payload };
      const emitted = await page.evaluate((p) => window.__emitMockStatus(p), frame);
      if (!emitted) throw new Error('Mock status emitter unavailable');
      await page.waitForTimeout(120);
    };

    const readState = async () =>
      page.evaluate(() => {
        const liveFill = document.getElementById('dome-live-fill');
        const speedLimit = document.getElementById('dome-speed-limit');
        const reloadButton = document.getElementById('reload-esc-button');
        return {
          hasLegacySlider: Boolean(document.getElementById('dome-slider')),
          hardwareState: document.getElementById('dome-hardware-state')?.textContent?.trim() || '',
          rotationStateClass: document.getElementById('dome-rotation-state')?.className || '',
          liveFillBackground: liveFill ? getComputedStyle(liveFill).backgroundColor : 'missing',
          rotationState: document.getElementById('dome-rotation-state')?.textContent?.trim() || '',
          speedText: document.getElementById('dome-speed-display')?.textContent?.trim() || '',
          pulseText: document.getElementById('dome-pulse-display')?.textContent?.trim() || '',
          feedbackText: document.getElementById('dome-feedback')?.textContent?.trim() || '',
          feedbackClass: document.getElementById('dome-feedback')?.className || '',
          liveFillOpacity: liveFill ? getComputedStyle(liveFill).opacity : 'missing',
          liveFillWidth: liveFill ? getComputedStyle(liveFill).width : 'missing',
          speedLimitDisabled: speedLimit ? speedLimit.disabled : null,
          reloadDisabled: reloadButton ? reloadButton.disabled : null,
        };
      });

    // What the droid reports it sent (#504), not the speed asked for: a
    // domeTargetSpeed beside it that disagrees must not reach the readout.
    const sent = (pct, pulseUs) => ({
      webControlEnabled: true, domeEnabled: true, domeTargetSpeed: 1,
      domeRotationSentPct: pct, domeRotationPulseUs: pulseUs,
    });
    await emitStatus(sent(-55, 1225));
    const reverse = await readState();

    await emitStatus(sent(0, 1500));
    const idle = await readState();

    await emitStatus(sent(72, 1860));
    const forward = await readState();

    if (reverse.hasLegacySlider) throw new Error('Legacy dome slider still present');
    // Exact strings, not includes(): the three readings are the whole of what
    // these elements say, and a substring match let 'Web control pill did not
    // become enabled' sit here passing against a pill that has never contained
    // the word 'enabled' (#399 slice 3).
    if (reverse.hardwareState !== 'on in Configuration') throw new Error('Dome hardware state not rendered');
    if (reverse.rotationState !== 'Anticlockwise' || reverse.speedText !== '-55%' || reverse.pulseText !== '1225 \u00b5s') {
      throw new Error('Anticlockwise status not rendered');
    }
    if (idle.rotationState !== 'Stopped' || idle.speedText !== '0%') throw new Error('Stopped status not rendered');
    if (forward.rotationState !== 'Clockwise' || forward.speedText !== '72%') throw new Error('Clockwise status not rendered');
    // A direction is a value, not a symptom: neither the word nor the bar may
    // take a signal color, and the bar is one color whichever way it goes
    // (GLOSSARY.md 'Status Color').
    if (reverse.rotationStateClass !== 'dome-rotation-state') throw new Error('Rotation state took a state class');
    if (forward.rotationStateClass !== 'dome-rotation-state') throw new Error('Rotation state took a state class');
    if (reverse.liveFillBackground !== forward.liveFillBackground) throw new Error('The live bar changed color with direction');

    console.log('DOME_PRESETS_START');
    console.log(JSON.stringify({ reverse, idle, forward }, null, 2));
    console.log('DOME_PRESETS_END');

    await page.screenshot({ path: '/tmp/dome-presets-audit.png', fullPage: true });
  } finally {
    await browser.close();
  }
})();
