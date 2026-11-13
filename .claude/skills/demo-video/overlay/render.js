#!/usr/bin/env node
// Renders overlay.html to a transparent PNG per frame, for ffmpeg to lay over
// the demo video (issue #509).
//
//   NODE_PATH=... node render.js <outDir>            every frame, at FPS
//   NODE_PATH=... AT=1.2,24,86 node render.js <outDir>   only those seconds
//
// A frame identical to the one before it (a held title, or nothing on screen)
// is not shot again: it is hard-linked, which is most of the track.
const crypto = require('node:crypto');
const fs = require('node:fs');
const path = require('node:path');

const { chromium } = require('playwright');

const FPS = Number(process.env.FPS || 30);
const out = process.argv[2];
if (!out) {
  console.error('usage: render.js <outDir>');
  process.exit(2);
}

(async () => {
  fs.mkdirSync(out, { recursive: true });
  const browser = await chromium.launch({ headless: true });
  try {
    const page = await (await browser.newContext({ viewport: { width: 1920, height: 1080 }, deviceScaleFactor: 1 })).newPage();
    await page.goto(`file://${path.join(__dirname, 'overlay.html')}`);
    // A face loads only once text needs it, and a title not yet on screen needs none: ask for each.
    await page.evaluate((families) => Promise.all(families.map((family) => document.fonts.load(`600 40px "${family}"`))), ['Space Grotesk', 'Orbitron', 'JetBrains Mono']);
    await page.evaluate(() => document.fonts.ready);
    const missing = await page.evaluate(() => ['Space Grotesk', 'Orbitron', 'JetBrains Mono'].filter((family) => !document.fonts.check(`600 40px "${family}"`)));
    if (missing.length) throw new Error(`fonts did not load: ${missing.join(', ')}`);
    const total = await page.evaluate(() => window.END.total);

    const shoot = async (t, file) => {
      await page.evaluate((seconds) => window.renderAt(seconds), t);
      const png = await page.screenshot({ omitBackground: true, type: 'png' });
      fs.writeFileSync(file, png);
      return png;
    };

    if (process.env.AT) {
      for (const t of process.env.AT.split(',').map(Number)) await shoot(t, path.join(out, `at-${t}.png`));
      return;
    }

    const frames = Math.round(total * FPS);
    let lastHash = null;
    let lastFile = null;
    let shot = 0;
    for (let i = 0; i < frames; i += 1) {
      const t = i / FPS;
      const file = path.join(out, `f_${String(i).padStart(5, '0')}.png`);
      fs.rmSync(file, { force: true });
      // What the page draws is decided by its DOM, so the markup is the frame's identity.
      await page.evaluate((seconds) => window.renderAt(seconds), t);
      const hash = crypto.createHash('sha1').update(await page.evaluate(() => document.body.innerHTML)).digest('hex');
      if (hash === lastHash) {
        fs.linkSync(lastFile, file);
      } else {
        fs.writeFileSync(file, await page.screenshot({ omitBackground: true, type: 'png' }));
        lastHash = hash;
        lastFile = file;
        shot += 1;
      }
    }
    console.log(`render: ${frames} frames at ${FPS} fps for ${total} s, ${shot} shot, ${frames - shot} linked`);
  } finally {
    await browser.close();
  }
})().catch((error) => {
  console.error(`render: ${error.stack || error}`);
  process.exit(1);
});
