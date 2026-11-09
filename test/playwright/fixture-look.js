#!/usr/bin/env node
// A fixture droid in a browser window, for a live look at the surfaces
// offline: nothing is checked and nothing is reported.
//
//   NODE_PATH=$HOME/.npm/_npx/e41f203b7505f1fb/node_modules \
//     node test/playwright/fixture-look.js
//
// Stages the default asset set into a fresh temporary directory with
// tools/stage_fsdata.py, serves it on a free port, and opens a headed browser
// at 1440x900 whose controller routes are answered by ./_lib/fixture_routes.js
// as the `dome` droid (its header says what that droid models), on dome.html.
// Every other surface is a click away in the same window.
//
// Why a browser and not only a server: the fixture droid lives in Playwright's
// routing, and the stage server answers /api/ with 404 on purpose so a route
// can own it. A browser opened anywhere else meets no droid at all.
//
// DROID=artoo|bench|dome picks another fixture droid, PAGE=<page>.html another
// first page. HEADLESS=true opens no window (the README's convention), so
// only Ctrl-C ends it. Closing the window, or Ctrl-C here, stops the server by
// its PID and removes the staged copy. Restage by running it again after a data/ edit.
const { spawn } = require('node:child_process');
const fs = require('node:fs');
const net = require('node:net');
const os = require('node:os');
const path = require('node:path');

const { chromium } = require('playwright');

const { install, DROIDS } = require('./_lib/fixture_routes.js');

const REPO = path.resolve(__dirname, '..', '..');
const DROID = process.env.DROID || 'dome';
const PAGE = process.env.PAGE || 'dome.html';
// Staging runs the whole gzip pass a firmware build runs, so it is given
// longer than a server that only has to bind.
const SERVE_DEADLINE_MS = 120000;

const fail = (message) => {
  console.error(`fixture-look: ${message}`);
  process.exit(1);
};

const freePort = () =>
  new Promise((resolve, reject) => {
    const server = net.createServer();
    server.once('error', reject);
    server.listen(0, '127.0.0.1', () => {
      const { port } = server.address();
      server.close(() => resolve(port));
    });
  });

// Stages and serves, and resolves once the page answers. stage_fsdata.py
// refuses a directory that is not empty, so the directory is a new one.
const stageAndServe = async (out, port) => {
  const child = spawn(
    'python3',
    [path.join(REPO, 'tools', 'stage_fsdata.py'), '--set', 'default', '--out', out, '--serve', String(port)],
    { cwd: REPO, stdio: ['ignore', 'inherit', 'pipe'] },
  );
  // The server logs every request to stderr; only the tail is kept, for a
  // failure message.
  const tail = [];
  child.stderr.on('data', (chunk) => {
    tail.push(...String(chunk).split('\n').filter(Boolean));
    tail.splice(0, Math.max(0, tail.length - 20));
  });
  let exited = null;
  child.on('exit', (code, signal) => {
    exited = signal || code;
  });
  const deadline = Date.now() + SERVE_DEADLINE_MS;
  for (;;) {
    if (exited !== null) fail(`stage_fsdata.py exited (${exited}):\n${tail.join('\n')}`);
    try {
      const response = await fetch(`http://127.0.0.1:${port}/${PAGE}`);
      if (response.ok) return child;
      child.kill('SIGTERM');
      fail(`the staged image answered ${PAGE} with ${response.status}`);
    } catch (_notListeningYet) {
      // Connection refused until staging is done and the server binds; the
      // deadline bounds it.
    }
    if (Date.now() > deadline) {
      child.kill('SIGTERM');
      fail(`the staged image did not answer on ${port} within ${SERVE_DEADLINE_MS / 1000} s:\n${tail.join('\n')}`);
    }
    await new Promise((resolve) => setTimeout(resolve, 250));
  }
};

const stopProcess = (child) =>
  new Promise((resolve) => {
    if (!child || child.exitCode !== null || child.signalCode !== null) return resolve();
    child.once('exit', () => resolve());
    child.kill('SIGTERM');
  });

(async () => {
  if (!DROIDS.includes(DROID)) fail(`no fixture droid called "${DROID}" (${DROIDS.join(', ')})`);
  const out = fs.mkdtempSync(path.join(os.tmpdir(), 'fixture-look-'));
  const port = await freePort();
  const base = `http://127.0.0.1:${port}`;
  let server = null;
  let browser = null;
  let droid = null;
  let finished = false;
  // One way out, whichever way the look ends.
  const finish = async (code) => {
    if (finished) return;
    finished = true;
    if (browser) await browser.close().catch(() => {});
    if (droid) await droid.close();
    await stopProcess(server);
    fs.rmSync(out, { recursive: true, force: true });
    process.exit(code);
  };
  process.on('SIGINT', () => finish(0));
  process.on('SIGTERM', () => finish(0));

  try {
    server = await stageAndServe(out, port);
    // Playwright's own SIGINT/SIGTERM handlers close the browser and exit 130
    // before finish() runs, which would leave the server and the staged copy
    // behind; finish() closes the browser itself.
    browser = await chromium.launch({
      headless: process.env.HEADLESS === 'true',
      handleSIGINT: false,
      handleSIGTERM: false,
    });
    browser.on('disconnected', () => finish(0));
    const context = await browser.newContext({ viewport: { width: 1440, height: 900 } });
    droid = await install(context, { droid: DROID });
    // Closing the last window ends the look: a browser Playwright launched
    // stays running with no window, so the page closing is the signal.
    const watch = (page) => page.on('close', () => {
      if (context.pages().length === 0) finish(0);
    });
    context.on('page', watch);
    const page = await context.newPage();
    await page.goto(`${base}/${PAGE}`);
    console.log(`fixture-look: the ${DROID} droid on ${base}/${PAGE} (server PID ${server.pid}). Close the window to stop.`);
  } catch (error) {
    console.error(`fixture-look: ${error.stack || error}`);
    await finish(1);
  }
})();
