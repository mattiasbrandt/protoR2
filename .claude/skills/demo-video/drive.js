#!/usr/bin/env node
// Drives the protoR2 demo's beats in a clean app window, for an OpenScreen take.
// The steps around it are SKILL.md, beside this file. Not a regression script.
//
//   NODE_PATH=$HOME/.npm/_npx/e41f203b7505f1fb/node_modules \
//     BEAT=all node .claude/skills/demo-video/drive.js
//
// BEAT is all (every beat, in the order of ORDER, in one take), one beat's
// name, or several with commas between. DRY=1 plays headless with no window and no take, and shoots each
// beat's last frame. BASE_URL is the board.
//
// Opens Chromium as an app window (no browser chrome) on the board, floats it
// at 2000x1250 device pixels (1600x1000 CSS at scale 1.25) on Hyprland
// workspace WS, and waits for the file `go` in the work directory (WORK,
// default tasks/demo-video). Then it plays the beats, writes each beat's start
// and end and every pointer action to log-<BEAT>.json, and waits for the file
// `done` before it closes the window. The times in the log are what the edit
// is placed by.
//
// The take records the system pointer, which a script never moves, so the page
// draws its own pointer and a ring on each press.
//
// The board is written to only by what ALLOWED lists, each line a beat's own
// press: a servo opened and closed, a light's mode and colour, a dome
// calibrate hold, the log level to Debug, and the pages' own reads. Every
// other request that is not a GET is refused in the browser and listed in the
// log. Nothing is connected to the board, so none of it moves anything.
const { execFileSync } = require('node:child_process');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');

const { chromium } = require('playwright');

const BASE = (process.env.BASE_URL || 'http://firebeetle2.local').replace(/\/$/, '');
const BEAT = process.env.BEAT || '1';
const WS = process.env.WS || '9';
const SEQ = process.env.SEQ || 'DM:FLUTTER';
// Where the go/done files, the logs and the dry-run shots go: gitignored tasks/.
const HERE = path.resolve(process.env.WORK || path.join(__dirname, '..', '..', '..', 'tasks', 'demo-video'));
const DRY = process.env.DRY === '1';
const GO = path.join(HERE, 'go');
const DONE = path.join(HERE, 'done');
const WAIT_GO_MS = 10 * 60 * 1000;
const WAIT_DONE_MS = 5 * 60 * 1000;
const WIN = { width: 2000, height: 1250, scale: 1.25 };

// Every write a beat may make, as [path, the shape of its decoded body].
const ALLOWED = [
  ['/api/console', /^command=(operations|system\.status\.health|help)$/], // the Console's reads
  ['/api/rc/debug', /^\{"enabled":(true|false)\}$/], // RC Control's live monitor, the page's own
  ['/api/servo', /^arm=GPIO 49&action=(open|close)$/], // servos beat
  ['/api/dome/cmd', /^cmd=DL:(FLD|RLD):(ALARM|RAINBOW):(DEFAULT|RED)$/], // lights beat
  ['/api/dome', /^speed=(1\.0|1|0)&durationMs=30000$/], // dome beat: a calibrate hold, then neutral
  ['/api/config', /^logLevel=4$/], // console beat
];

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

const waitForFile = async (file, timeoutMs) => {
  const deadline = Date.now() + timeoutMs;
  while (!fs.existsSync(file)) {
    if (Date.now() > deadline) throw new Error(`no ${path.basename(file)} file within ${timeoutMs / 1000} s`);
    await sleep(100);
  }
};

const hypr = (...args) => execFileSync('hyprctl', args, { encoding: 'utf8' });

// Finds the app window by its process, floats it at the take's size on WS.
const placeWindow = async (pid) => {
  const deadline = Date.now() + 15000;
  let win = null;
  while (!win) {
    win = JSON.parse(hypr('clients', '-j')).find((c) => c.pid === pid);
    if (!win && Date.now() > deadline) throw new Error(`Hyprland shows no window for pid ${pid}`);
    if (!win) await sleep(200);
  }
  // Hyprland 0.56 dispatches Lua (hl.dsp.*); the old "dispatch name args" form is refused.
  const window = `window = "address:${win.address}"`;
  const dispatch = (call) => {
    const out = hypr('dispatch', call).trim();
    if (out !== 'ok') throw new Error(`hyprctl dispatch ${call}: ${out}`);
  };
  dispatch(`hl.dsp.window.move({ workspace = "${WS}", follow = true, ${window} })`);
  dispatch(`hl.dsp.window.float({ action = "on", ${window} })`);
  dispatch(`hl.dsp.window.resize({ x = ${WIN.width}, y = ${WIN.height}, ${window} })`);
  dispatch(`hl.dsp.window.center({ ${window} })`);
  dispatch(`hl.dsp.focus({ ${window} })`);
  await sleep(600);
  // The real pointer is burned into the take; left of the window it is cropped out.
  dispatch('hl.dsp.cursor.move({ x = 300, y = 760 })');
  const placed = JSON.parse(hypr('clients', '-j')).find((c) => c.address === win.address);
  const monitor = JSON.parse(hypr('monitors', '-j')).find((m) => m.id === placed.monitor);
  return {
    title: placed.title,
    at: placed.at,
    size: placed.size,
    monitor: { name: monitor.name, x: monitor.x, y: monitor.y, width: monitor.width, height: monitor.height },
  };
};

(async () => {
  fs.mkdirSync(path.join(HERE, 'shots'), { recursive: true });
  for (const file of [GO, DONE]) fs.rmSync(file, { force: true });
  const profile = fs.mkdtempSync(path.join(os.tmpdir(), 'protor2-demo-'));
  let context = null;
  const events = [];
  const refused = [];
  const allowed = [];
  try {
    context = await chromium.launchPersistentContext(profile, {
      // DRY=1: no window and no take; plays the beats and shoots each one's last frame.
      headless: DRY,
      viewport: DRY ? { width: 1600, height: 1000 } : null,
      args: [
        `--app=${BASE}/#home`,
        `--window-size=${WIN.width},${WIN.height}`,
        `--force-device-scale-factor=${WIN.scale}`,
        '--hide-scrollbars',
      ],
    });
    // The take records the system pointer, which a script never moves, so the
    // page draws one: an arrow that follows the scripted mouse, and a ring on a press.
    await context.addInitScript(() => {
      const install = () => {
        if (document.getElementById('demo-pointer')) return;
        const style = document.createElement('style');
        style.textContent = `
          #demo-pointer { position: fixed; left: 0; top: 0; width: 26px; height: 26px; z-index: 2147483647;
            pointer-events: none; transform: translate(800px, 500px); will-change: transform; }
          .demo-ring { position: fixed; width: 44px; height: 44px; margin: -22px 0 0 -22px; border-radius: 50%;
            border: 2px solid rgba(96, 165, 250, 0.95); z-index: 2147483646; pointer-events: none;
            animation: demo-ring 520ms ease-out forwards; }
          @keyframes demo-ring { from { transform: scale(0.25); opacity: 1; } to { transform: scale(1.15); opacity: 0; } }
          html, html * { cursor: none !important; }`;
        document.documentElement.appendChild(style);
        const pointer = document.createElement('div');
        pointer.id = 'demo-pointer';
        pointer.innerHTML = '<svg viewBox="0 0 24 24" width="26" height="26"><path d="M4 2.5v17l4.6-4.3 3 6.6 2.7-1.2-3-6.5h6.4z" fill="#f8fafc" stroke="#0b1220" stroke-width="1.4" stroke-linejoin="round"/></svg>';
        document.documentElement.appendChild(pointer);
        window.addEventListener('mousemove', (event) => {
          pointer.style.transform = `translate(${event.clientX - 4}px, ${event.clientY - 2}px)`;
        }, true);
        window.addEventListener('mousedown', (event) => {
          const ring = document.createElement('div');
          ring.className = 'demo-ring';
          ring.style.left = `${event.clientX}px`;
          ring.style.top = `${event.clientY}px`;
          document.documentElement.appendChild(ring);
          setTimeout(() => ring.remove(), 600);
        }, true);
      };
      if (document.documentElement) install();
      document.addEventListener('DOMContentLoaded', install);
    });
    const page = context.pages()[0] || (await context.waitForEvent('page'));
    await context.route('**/*', (route) => {
      const request = route.request();
      if (request.method() === 'GET') return route.continue();
      const where = new URL(request.url()).pathname;
      const body = decodeURIComponent((request.postData() || '').replace(/\+/g, ' '));
      const wanted = ALLOWED.some(([path_, shape]) => path_ === where && shape.test(body));
      // A dry run leaves the log level alone, so the take still finds it at Info.
      if (wanted && !(DRY && where === '/api/config')) {
        allowed.push(`${request.method()} ${where} ${body}`);
        return route.continue();
      }
      refused.push(`${request.method()} ${new URL(request.url()).pathname}`);
      return route.abort();
    });

    const pid = DRY ? 0 : context.browser()?.process?.()?.pid
      ?? Number(execFileSync('pgrep', ['-o', '-f', `^[^ ]*chrom[^ ]* .*--user-data-dir=${profile}`], { encoding: 'utf8' }).trim().split('\n')[0]);
    const placed = DRY ? { at: [0, 0], size: [1600, 1000] } : await placeWindow(pid);

    // The pointer, where the page last had it, in CSS pixels of the page.
    let at = { x: 800, y: 500 };
    const frac = async (x, y) => page.evaluate(([cx, cy]) => {
      const border = (outerWidth - innerWidth) / 2;
      return {
        x: Math.min(1, Math.max(0, (border + cx) / outerWidth)),
        y: Math.min(1, Math.max(0, (outerHeight - innerHeight - border + cy) / outerHeight)),
      };
    }, [x, y]);
    const log = async (x, y, click = false) => {
      const f = await frac(x, y);
      events.push({ at: Date.now(), x: f.x, y: f.y, ...(click ? { click: true } : {}) });
    };
    const centre = async (target) => {
      const locator = typeof target === 'string' ? page.locator(target).first() : target;
      await locator.waitFor({ state: 'visible', timeout: 15000 });
      const box = await locator.boundingBox();
      if (!box) throw new Error(`no box for ${target}`);
      return { x: box.x + box.width / 2, y: box.y + box.height / 2 };
    };
    // Glides the real page pointer there (hover styles follow) and logs the arrival.
    const moveTo = async (x, y, ms = 600) => {
      const steps = Math.max(2, Math.round(ms / 16));
      for (let i = 1; i <= steps; i += 1) {
        const t = i / steps;
        const ease = t * t * (3 - 2 * t);
        await page.mouse.move(at.x + (x - at.x) * ease, at.y + (y - at.y) * ease);
        await sleep(16);
      }
      at = { x, y };
      await log(x, y);
    };
    const hover = async (target, ms) => {
      const p = await centre(target);
      await moveTo(p.x, p.y, ms);
    };
    const click = async (target, ms) => {
      const p = await centre(target);
      await moveTo(p.x, p.y, ms);
      await sleep(180);
      await page.mouse.click(p.x, p.y);
      await log(p.x, p.y, true);
    };
    const dragBy = async (target, dx, ms = 900) => {
      const p = await centre(target);
      await moveTo(p.x, p.y);
      await sleep(200);
      await page.mouse.down();
      await log(p.x, p.y, true);
      const steps = Math.round(ms / 16);
      for (let i = 1; i <= steps; i += 1) {
        const t = i / steps;
        await page.mouse.move(p.x + dx * t * t * (3 - 2 * t), p.y);
        await sleep(16);
      }
      await page.mouse.up();
      at = { x: p.x + dx, y: p.y };
      await log(at.x, at.y);
    };
    // Scrolls the page by wheel, at the pointer, so the motion is even.
    const scrollBy = async (pixels, ms) => {
      const steps = Math.max(1, Math.round(ms / 16));
      const each = pixels / steps;
      for (let i = 0; i < steps; i += 1) {
        await page.mouse.wheel(0, each);
        await sleep(16);
      }
      await log(at.x, at.y);
    };
    // Wheels the page to an exact scroll position at an even 1300 px/s.
    const scrollToY = async (y) => {
      const from = await page.evaluate(() => scrollY);
      await scrollBy(y - from, Math.max(500, Math.abs(y - from) / 1.3));
      const landed = await page.evaluate(() => scrollY);
      if (Math.abs(landed - y) > 4) await page.mouse.wheel(0, y - landed);
    };
    const open = async (hash, ready, { fresh = false } = {}) => {
      await page.goto(`${BASE}/${hash}`);
      // A page opened from another page waits for the next status frame before
      // it knows what the droid last sent; a fresh load asks at once.
      if (fresh) await page.reload();
      await page.locator(ready).first().waitFor({ state: 'visible', timeout: 20000 });
      await sleep(2500);
    };
    const tuned = async () => {
      await open('#seq', `.seq-item[data-seq-name="${SEQ}"]`);
      await page.click(`.seq-item[data-seq-name="${SEQ}"] [data-action="tune"]`);
      await page.waitForSelector('#seq-editor-timeline .tl-item', { timeout: 15000 });
      await sleep(1200);
    };

    const marks = [];
    // Each beat: `stage` runs before the take, `play` during it. The first
    // click of a take comes after 2.8 s, since OpenScreen keeps the opening wide.
    const BEATS = {
      // Dashboard: the droid picture, front then rear, then the Status Plate.
      dashboard: {
        stage: () => open('#home', '.app-shell, body[data-page="home"]'),
        play: async () => {
          await log(at.x, at.y);
          await sleep(2800);
          await click('.bv-tab[data-face-tab="rear"]', 900);
          await sleep(1600);
          await click('.bv-tab[data-face-tab="front"]', 500);
          await sleep(1400);
          await hover('.status-chip-label', 1100);
          await sleep(1500);
        },
      },
      // Configuration: one stop per category, top to bottom: the board, the
      // Droid Build (dome, then body), then every hardware category's products.
      configuration: {
        stage: () => open('#configuration', 'body[data-page="configuration"]'),
        play: async () => {
          await log(at.x, at.y);
          await sleep(2800);
          await moveTo(760, 520, 800);
          await sleep(700);
          for (const [y, hold] of [[680, 1400], [1130, 1600], [1620, 1400], [2170, 1500], [2590, 1300], [2998, 1400], [3538, 1500], [3948, 1300], [4290, 1700]]) {
            await scrollToY(y);
            marks.push({ beat: `configuration@${y}`, from: Date.now(), to: Date.now() + hold });
            await sleep(hold);
          }
        },
      },
      // Wiring: the wires on the board, then the one table.
      wiring: {
        stage: () => open('#wiring', 'body[data-page="wiring"]'),
        play: async () => {
          await log(at.x, at.y);
          await sleep(2800);
          await hover(page.locator('.parts-name', { hasText: 'Upper utility arm' }), 900);
          await sleep(1500);
          await hover(page.locator('.parts-name', { hasText: 'Dome link' }), 700);
          await sleep(1300);
          await scrollBy(520, 2200);
          await sleep(1700);
        },
      },
      // Parts: pick a part in the list and see it on the picture.
      parts: {
        stage: () => open('#parts', 'body[data-page="parts"]'),
        play: async () => {
          await log(at.x, at.y);
          await sleep(2800);
          await click('.bv-list-pick[data-pick-marker="utilUp"]', 900);
          await sleep(1900);
          await click('.bv-list-pick[data-pick-marker="utilLo"]', 700);
          await sleep(1900);
        },
      },
      // Servos: the first Output told to open, then to close.
      servos: {
        stage: () => open('#servo', '.outputs-go[data-action="open"]'),
        play: async () => {
          await log(at.x, at.y);
          await sleep(2800);
          await click(page.locator('.outputs-go[data-action="open"]').first(), 900);
          await sleep(2200);
          await click(page.locator('.outputs-go[data-action="close"]').first(), 600);
          await sleep(2200);
        },
      },
      // Lights: a mode and a colour on the Front Logic Displays, a mode on the rear.
      lights: {
        stage: () => open('#lights', '.light-mode'),
        play: async () => {
          await log(at.x, at.y);
          await sleep(2800);
          await click(page.locator('.light-mode', { hasText: 'Alarm' }).first(), 900);
          await sleep(1500);
          await click(page.locator('.light-color[data-color="red"]').first(), 600);
          await sleep(1600);
          await click(page.locator('.light-mode', { hasText: 'Rainbow' }).nth(1), 800);
          await sleep(1900);
        },
      },
      // Dome Rotation: a calibrate hold and back to neutral, then its settings.
      dome: {
        // Fresh: opened from another page, the calibrate row stays hidden (no
        // status frame yet says the droid sends a pulse).
        stage: () => open('#dome', '#dome-calibrate-holds button', { fresh: true }),
        play: async () => {
          await log(at.x, at.y);
          await sleep(2800);
          await click(page.locator('#dome-calibrate-holds button').first(), 900);
          await sleep(2000);
          await click(page.locator('#dome-calibrate-holds button', { hasText: 'Neutral' }), 500);
          await sleep(1300);
          await moveTo(760, 620, 500);
          await scrollBy(330, 1500);
          await sleep(2200);
        },
      },
      // RC Control: the radio, the RC Receiver, the bindings, the sticks, the sources.
      rc: {
        stage: () => open('#rc', 'body[data-page="rc"]'),
        play: async () => {
          await log(at.x, at.y);
          await sleep(2800);
          await moveTo(760, 560, 600);
          for (const [y, hold] of [[440, 1500], [900, 1700], [1230, 1700], [1650, 2000]]) {
            await scrollToY(y);
            marks.push({ beat: `rc@${y}`, from: Date.now(), to: Date.now() + hold });
            await sleep(hold);
          }
        },
      },
      // Sequences: drag a block on the timeline, then one Undo. Nothing is saved.
      sequences: {
        stage: tuned,
        play: async () => {
          await log(at.x, at.y);
          await sleep(2800);
          await dragBy('#seq-editor-timeline .tl-row[data-lane="pie1"] .tl-item', 150);
          await sleep(1800);
          await click('#seq-editor-undo', 900);
          await sleep(1600);
        },
      },
      // Dashboard Console: the log level to Debug, lines arriving, then a
      // question typed to the droid and its answer. Set the level to Info before
      // the take: the pill cycles, and from Debug the next press is Error.
      console: {
        stage: () => open('#home', '#log-console'),
        play: async () => {
          await log(at.x, at.y);
          await sleep(1200);
          await moveTo(760, 600, 500);
          await scrollToY(885);
          await sleep(1300);
          const level = (await page.locator('#log-level-pill').textContent()).trim();
          marks.push({ beat: `console level was ${level}`, from: Date.now(), to: Date.now() });
          if (level === 'Info' && !DRY) await click('#log-level-pill', 800);
          else await hover('#log-level-pill', 800);
          await sleep(2600);
          await click('#log-command-input', 800);
          await page.keyboard.type('system.status.health', { delay: 70 });
          await sleep(400);
          await page.keyboard.press('Enter');
          marks.push({ beat: 'console command sent', from: Date.now(), to: Date.now() });
          await sleep(3400);
        },
      },
    };
    // The video's order. The console is last: its end is what the end card covers.
    const ORDER = ['dashboard', 'configuration', 'wiring', 'parts', 'servos', 'lights', 'dome', 'rc', 'sequences', 'console'];
    // BEAT=all plays every beat in one take, so the picker is answered once;
    // the staging between two beats is trimmed out by the times logged here.
    const order = BEAT === 'all' ? ORDER : BEAT.split(',');
    for (const id of order) if (!BEATS[id]) throw new Error(`no beat ${id} (${Object.keys(BEATS).join(', ')}, all)`);

    await BEATS[order[0]].stage();
    await page.mouse.move(at.x, at.y);
    console.log(`drive: beat ${order[0]} staged, window "${await page.title()}" at ${placed.at} size ${placed.size}. Waiting for go.`);
    if (!DRY) await waitForFile(GO, WAIT_GO_MS);
    const startedAt = Date.now();
    for (const id of order) {
      if (id !== order[0]) await BEATS[id].stage();
      const from = Date.now();
      await BEATS[id].play();
      marks.push({ beat: id, from, to: Date.now() });
      if (DRY) await page.screenshot({ path: path.join(HERE, 'shots', `dry-${id}.png`) });
      console.log(`drive: beat ${id} played`);
    }
    const endedAt = Date.now();
    fs.writeFileSync(path.join(HERE, `log-${BEAT}.json`), JSON.stringify({ beat: BEAT, startedAt, endedAt, marks, window: placed, events, refused, allowed }, null, 1));
    console.log(`drive: ${order.length} beat(s) played in ${((endedAt - startedAt) / 1000).toFixed(1)} s, ${events.length} pointer events, ${refused.length} writes refused. Waiting for done.`);
    if (!DRY) await waitForFile(DONE, WAIT_DONE_MS);
  } finally {
    if (context) await context.close().catch(() => {});
    fs.rmSync(profile, { recursive: true, force: true });
    for (const file of [GO, DONE]) fs.rmSync(file, { force: true });
  }
})().catch((error) => {
  console.error(`drive: ${error.stack || error}`);
  process.exit(1);
});
