#!/usr/bin/env node
// The end card on its own: the frames of the looping splash, the logo, and the
// wordmark on a transparent ground. ffmpeg makes the GIF from the frames
// (SKILL.md, "Splash and logo").
//
//   NODE_PATH=... node splash.js <outDir>
//
// Writes <outDir>/splash/f_%04d.png (20 fps, SECONDS long, 1920x1080),
// <outDir>/protoR2-logo.png and <outDir>/protoR2-wordmark.png.
const fs = require('node:fs');
const path = require('node:path');

const { chromium } = require('playwright');

const FPS = 20;
const SECONDS = 6.5;
// Where the card has fully landed and no light is crossing the wordmark.
const STILL_AT = 3.6;
const out = process.argv[2];
if (!out) {
  console.error('usage: splash.js <outDir>');
  process.exit(2);
}

(async () => {
  fs.mkdirSync(path.join(out, 'splash'), { recursive: true });
  const browser = await chromium.launch({ headless: true });
  try {
    const context = await browser.newContext({ viewport: { width: 1920, height: 1080 }, deviceScaleFactor: 1 });
    const open = async (query) => {
      const page = await context.newPage();
      await page.goto(`file://${path.join(__dirname, 'overlay.html')}?${query}`);
      // A face loads only once text needs it, and a title not yet on screen needs none: ask for each.
      await page.evaluate((families) => Promise.all(families.map((family) => document.fonts.load(`600 40px "${family}"`))), ['Orbitron', 'JetBrains Mono']);
      await page.evaluate(() => document.fonts.ready);
      const missing = await page.evaluate(() => ['Orbitron', 'JetBrains Mono'].filter((family) => !document.fonts.check(`600 40px "${family}"`)));
      if (missing.length) throw new Error(`fonts did not load: ${missing.join(', ')}`);
      return page;
    };
    const at = (page, seconds) => page.evaluate((s) => window.renderAt(window.END.from + s), seconds);

    const solo = await open('solo');
    for (let i = 0; i < Math.round(SECONDS * FPS); i += 1) {
      await at(solo, i / FPS);
      await solo.screenshot({ path: path.join(out, 'splash', `f_${String(i).padStart(4, '0')}.png`) });
    }
    await at(solo, STILL_AT);
    await solo.screenshot({ path: path.join(out, 'protoR2-logo.png') });

    const mark = await open('mark');
    await at(mark, STILL_AT);
    const box = await mark.evaluate(() => {
      const rects = ['.eyebrow', '.mark', '.rule'].map((sel) => document.querySelector(`#end ${sel}`).getBoundingClientRect());
      const left = Math.min(...rects.map((r) => r.left)) - 60;
      const top = Math.min(...rects.map((r) => r.top)) - 50;
      return { x: left, y: top, width: Math.max(...rects.map((r) => r.right)) + 60 - left, height: Math.max(...rects.map((r) => r.bottom)) + 50 - top };
    });
    await mark.screenshot({ path: path.join(out, 'protoR2-wordmark.png'), omitBackground: true, clip: box });
    console.log(`splash: ${Math.round(SECONDS * FPS)} frames, the logo and the wordmark (${Math.round(box.width)}x${Math.round(box.height)}) in ${out}`);
  } finally {
    await browser.close();
  }
})().catch((error) => {
  console.error(`splash: ${error.stack || error}`);
  process.exit(1);
});
