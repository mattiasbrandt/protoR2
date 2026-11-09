// Browser-side stand-ins for the controller routes tools/serve_editor_fixture.py
// does not answer, for every script under test/playwright/ that takes FIXTURE=1:
// the rule scripts through ./checks.js runCheck(), and the older scripts
// (shell/stop-every-surface.js, shell/status-plate-truth.js,
// parts/parts-surface.js, console-sweep.js) that install it themselves, and
// for ../fixture-look.js, which opens a fixture droid for a live look.
//
// Loaded ONLY when FIXTURE=1, or by fixture-look.js, which is always offline.
// Against the live controller nothing here runs,
// so a live run is the script and the droid and nothing in between.
//
// Why a module and not a fixture-server edit: the fixture server is shared by
// every other script and deliberately 404s any /api/* route it does not have
// (its header, #261). These answers are shaped for these scripts' needs, so
// they live beside them and ride on Playwright's routing instead.
//
// THE THREE DROIDS. install(context, { droid }) answers as one of three droids.
// Every route below is answered for each; only the shapes of a few answers
// differ, and each script keeps the droid its checks were proved against.
//   'artoo' (the default) - five Artoo-style Outputs from include/board_outputs.h
//       (ARM1 carrying two body panels, calibrated; ARM2 wired with nothing on
//       it; AUX1 and AUX2 not wired; AUX3 the CBI on an LED strip), a
//       configured droid's config (components, Droid Build, a guided Setup
//       record of a droid configured before it existed), and a full identity
//       and status frame.
//   'bench' - five wired ARMs with no light, ARM1 ganged to two body panels and
//       FOLLOWING: its commanded width alternates 1400/1600 on every Outputs
//       read, so a once-a-second follow actually changes a value, which is
//       what makes "only values change" a test rather than a formality. The
//       config is the older-firmware answer with no droidBuild key, so the
//       Parts picture draws every marker; the status frame carries the six
//       fields data/live_reading.js requires and just enough for every Status
//       Plate chip to read a value.
//   'dome' - the artoo droid with Dome Rotation on, for a live look at what
//       #504 built (test/playwright/fixture-look.js starts it): switched on and
//       started with (`activeToggles`), its Settings at the firmware's
//       defaults (src/config_settings.cpp) but for Positive turns, set to
//       clockwise, and a Dome ESC that reports a pulse - neutral from the
//       first frame, as the droid does from start-up. It answers
//       POST /api/dome and the Dome Rotation Settings on POST /api/config
//       (below). Nothing else differs from artoo.
//
// Other options: `estop: true` starts the droid latched (and its enabled
// Outputs already let go); `textPlainPhotos: [ids]` answers those registry
// photographs as text/plain, for a self-test.
//
// FIXTURE_FITTED=<part ids, comma-separated> in the environment fits those
// Parts on whichever droid a script runs, so a script with no option of its
// own can meet a droid that carries a Common Addition - an arm is on the Parts
// picture only once it is fitted. The artoo droid adds them to its Droid
// Build; the bench droid, which has none, gets an MK4 Complex body and dome
// with those and the Parts on its Outputs fitted. Unset, both droids answer exactly as above.
//
// THE STATUS STREAM is the one route page.route cannot fake faithfully:
// route.fulfill() hands over a finished body, so an EventSource reading it
// sees the stream END, errors and reconnects - a flapping link, which is the
// exact thing status-plate-truth.js measures. So /api/events is continued to a
// small real SSE server started here, which holds the connection open.
//
// The same small server also takes POST /api/estop and /api/estop/clear, so a
// script can latch or release the estop from OUTSIDE the browser the way a
// second client or the Console would on the bench: page.route never sees a
// request made with Playwright's own request API, so that write has to reach
// something that is not a route.
//
// One droid, many browsers: install() routes the first context and hands back
// addContext(), which routes another context against the SAME state and the
// same stream, so a fresh browser context meets the droid the first one left
// (the estop it latched, the sleep it asked for).
//
// WHAT IT MODELS, AND FROM WHERE. Each behaviour below is the firmware's, read
// from the source named beside it, so a script that passes here passes for the
// reason it would on the droid:
//   - GET /api/identity/components is parsed out of include/component_registry.inc
//     at install time, not typed here. `included` follows the artoo_esp32 build.
//   - /<id>.webp answers image/webp for every picture the set carries whose
//     name has the safe shape (lowercase letters, digits, underscore): one
//     handler claims the shape, not a list of ids, so Droid Build pictures
//     such as mrbaddeley answer image/webp too (include/web_webp.h
//     webPathIsWebpPicture, src/web/web_request_psychic.cpp, #355 finding 4).
//   - POST /api/servo hold/release: a press takes the Output, `refresh=1` only
//     refreshes a hold that stands and is dropped otherwise, and a hold nobody
//     refreshes for SERVO_HOLD_EXPIRY_MS (3000, include/config.h) goes limp with
//     `limp: "expiry"` (include/servo_hold.h, src/tasks/servo_task.cpp holdOutput).
//   - POST /api/estop latches and releases every ENABLED Output with
//     `limp: "estop"` (releaseAllOutputs, include/servo_halt.h). Enabled is the
//     wired tick on a row that carries no light (isOutputEnabled's lit mask). A
//     status is pushed on the first trigger only (src/failsafe_gate.cpp
//     failsafeTrigger, requestStatusBroadcastNow); a repeat is idempotent.
//   - POST /api/estop/clear releases the latch and pushes a status only when
//     it was set (src/failsafe_gate.cpp failsafeClearEstop); the Outputs the
//     latch let go stay limp until something takes them again.
//   - A hold under a latched estop is answered 200 and changes nothing: the
//     route queues it and ServoTask refuses it (src/web/api_servo.cpp).
//   - POST /api/config is accepted in exactly two shapes. JSON whose only key
//     is `outputs` saves a row's ledCount and its release time (0..60000 ms,
//     #443) and answers the config (sendConfigSnapshot()). A body that is
//     nothing but guidedSetupVisited=<step> (data/setup.js saveVisited) is
//     taken and kept in `state.accepted`. On the dome droid only, a form
//     body whose every field is a Dome Rotation Setting is taken too, held
//     to the Setting's range, words or true/false and to the pulse order and
//     the random pauses' order, refused in the firmware's envelope (field,
//     reason, accepts; src/config_settings.cpp, src/web/api_config_apply.cpp),
//     and kept in `state.accepted`. Any other POST /api/config, including a
//     Part move, is refused. The fixture droid does not move Parts.
//   - POST /api/dome, on the dome droid only (src/web/api_drive.cpp
//     handleDomeSpeedPost, src/tasks/dome_rotation_task.cpp): `speed` and an
//     optional `durationMs` refused as the droid refuses them, 423 asleep, and
//     taken and discarded under the estop. A turn puts out the pulse
//     domeSpeedToPulseUs() gives under the saved pulses, Speed limit and
//     Smallest turn, and reports the share domePulseToShare() gives
//     (include/dome_math.h) as domeRotationSentPct, domeRotationPulseUs and the
//     domeRotation entry, and pushes a status. It stops after `durationMs`, or
//     0.5 s after an untimed command; the estop and Sleep Mode stop it at once.
//     A stop reports neutral as it was at the last write, so a pulse saved
//     while stopped shows from the next turn, as on the droid. Not modelled:
//     Pause before reversing (a turn the other way goes at once here), random
//     movement, and the Dome Bearing, which stays unknown.
//   - POST /api/sleep and /api/wake answer {ok, sleepMode, changed}
//     (docs/api.md) and push a status only when the posture changed
//     (src/web/api_system.cpp).
//   - A client admitted to /api/events is pushed a status, and so is every
//     other open client (src/web/api_events.cpp).
//   - GET /api/rc and GET /api/rc/map answer as docs/api.md gives them for a
//     droid with no receiver switched on. Without them the RC surface's
//     sections fail and the bootstrap retries them on its own clock, which is
//     a request nothing on the surface asked for - and exactly what a "leaving
//     a surface stops its polling" count must not be confused by.
//   - GET /api/seq/list and /api/seq/builtins answer an empty JSON array each
//     (docs/api.md): no Learned Sequences, and the factory list left empty
//     rather than invented.
// Any other write is refused with 400 and kept in `state.writes`, so a script
// that asserts "no writes" has something to read. Any other read goes to the
// fixture server, which 404s it the way the controller answers a route it does
// not have.
//
// Shapes follow docs/api.md; they are a plausible droid with nothing but USB
// connected, not a copy of any one board's answer.
const http = require('node:http');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');

const REPO = path.resolve(__dirname, '..', '..', '..');
const ASSETS = path.join(REPO, 'data', 'asset-sets', 'default');
const HOLD_EXPIRY_MS = 3000;
const DROIDS = ['artoo', 'bench', 'dome'];
// How long an untimed dome turn runs with no command behind it
// (DOME_COMMAND_TIMEOUT_MS, src/tasks/dome_rotation_task.cpp).
const DOME_COMMAND_TIMEOUT_MS = 500;
// The longest timed dome turn (DOME_TIMED_TURN_MAX_MS, include/robot_state.h).
const DOME_TIMED_TURN_MAX_MS = 30000;

// The Dome Rotation Settings as src/config_settings.cpp declares them, in its
// order: form name, key under GET /api/config's `domeRotation`, and the rule
// with the firmware's default. A dome droid's GET carries every one.
const DOME_ROTATION_SETTINGS = [
  ['domeRotationNeutralUs', 'neutralUs', { lo: 1000, hi: 2000, def: 1500 }],
  ['domeRotationMinPulseUs', 'minPulseUs', { lo: 1000, hi: 2000, def: 1000 }],
  ['domeRotationMaxPulseUs', 'maxPulseUs', { lo: 1000, hi: 2000, def: 2000 }],
  ['domeRotationSpeedLimitPct', 'speedLimitPct', { lo: 0, hi: 100, def: 100 }],
  ['domeRotationSmallestTurnUs', 'smallestTurnUs', { lo: 0, hi: 1000, def: 0 }],
  ['domeRotationReversePauseMs', 'reversePauseMs', { lo: 0, hi: 2000, def: 0 }],
  ['domeRotationRndEnable', 'rndEnable', { bool: true, def: false }],
  ['domeRotationRndSpeedPct', 'rndSpeedPct', { lo: 5, hi: 100, def: 30 }],
  ['domeRotationRndPauseMin', 'rndPauseMin', { lo: 1, hi: 120, def: 6 }],
  ['domeRotationRndPauseMax', 'rndPauseMax', { lo: 1, hi: 120, def: 12 }],
  ['domeRotationRndMoveMs', 'rndMoveMs', { lo: 500, hi: 10000, def: 2500 }],
  ['domeRotationFullTurnMs', 'fullTurnMs', { lo: 0, hi: 60000, def: 0 }],
  ['domeRotationFullTurnPct', 'fullTurnPct', { lo: 0, hi: 100, def: 0 }],
  ['domeRotationPositiveTurn', 'positiveTurn', { words: ['unset', 'cw', 'ccw'], def: 'unset' }],
];

// include/dome_math.h domeSpeedToPulseUs(), in the float steps the droid
// takes and truncated as it truncates: the same port data/dome.js makes for
// its hints (sentPulseUs), which cannot be required from here.
const domeSpeedToPulseUs = (speed, neutralUs, minUs, maxUs, limitPct, smallestUs) => {
  const f = Math.fround;
  if (!(minUs <= neutralUs && neutralUs <= maxUs)) return neutralUs;
  const s = Math.max(-1, Math.min(1, speed));
  const scale = f(limitPct / 100);
  const range = f((s > 0 ? maxUs - neutralUs : neutralUs - minUs) * scale);
  if (s === 0) return neutralUs;
  let pulse;
  if (smallestUs === 0) {
    pulse = Math.trunc(f(neutralUs + f(s * range)));
  } else {
    const edge = Math.min(f(smallestUs), range);
    const start = f(range - edge) >= 1 ? f(edge + 1) : edge;
    const dist = Math.trunc(f(start + f(Math.abs(s) * f(range - start))));
    pulse = s > 0 ? neutralUs + dist : neutralUs - dist;
  }
  return Math.max(minUs, Math.min(maxUs, pulse));
};

// include/dome_math.h domePulseToShare(): the pulse as a signed share of the
// ESC's full output, measured from Smallest turn's edge. This, not the speed
// asked for, is what the droid reports it sent.
const domePulseToShare = (pulseUs, neutralUs, minUs, maxUs, smallestUs) => {
  const ofSide = (distUs, sideUs) => {
    if (distUs >= sideUs) return 1;
    const edgeUs = Math.min(smallestUs, sideUs);
    if (distUs <= edgeUs) return 0;
    return Math.fround((distUs - edgeUs) / (sideUs - edgeUs));
  };
  if (pulseUs > neutralUs && maxUs > neutralUs) return ofSide(pulseUs - neutralUs, maxUs - neutralUs);
  if (pulseUs < neutralUs && neutralUs > minUs) return -ofSide(neutralUs - pulseUs, neutralUs - minUs);
  return 0;
};

// The Component Registry, read from the one file the firmware builds it from.
const readRegistry = () => {
  const source = fs.readFileSync(path.join(REPO, 'include', 'component_registry.inc'), 'utf8');
  const categories = [];
  const categoryId = new Map();
  for (const match of source.matchAll(/^PA_COMPONENT_CATEGORY\((\w+),\s*"([^"]+)",\s*"([^"]+)",\s*(nullptr|"[^"]+")\)/gm)) {
    categoryId.set(match[1], match[2]);
    categories.push({ id: match[2], name: match[3], member_key: match[4] === 'nullptr' ? null : match[4].slice(1, -1) });
  }
  const parts = [];
  const partRe = /^PA_COMPONENT_PART\(\s*(\d+),\s*"([^"]+)",\s*"([^"]+)",\s*(\w+),\s*"([^"]+)",\s*COMPONENT_STATUS_(\w+),([\s\S]*?)\)\s*$/gm;
  for (const match of source.matchAll(partRe)) {
    const status = match[6].toLowerCase();
    const tail = match[7];
    // The last argument is `included`: a literal, the board test, or a
    // capability macro that is 1 on the artoo_esp32 build.
    const includedExpr = tail.slice(tail.lastIndexOf(',') + 1).trim();
    let included;
    if (includedExpr === '1') included = true;
    else if (includedExpr === '0') included = false;
    else if (includedExpr.includes('PA_BOARD_ARTOO_ESP32')) included = true;
    else if (includedExpr.includes('PA_BOARD_FIREBEETLE2')) included = false;
    else if (includedExpr.startsWith('PA_CAP_')) included = true;
    else throw new Error(`component_registry.inc: cannot read "included" for ${match[2]}: ${includedExpr}`);
    // The first argument after `status` is `confirmed_on_droid`, one of two
    // words (include/component_registry.inc column notes).
    const confirmedWord = tail.slice(0, tail.indexOf(',')).trim();
    if (!['COMPONENT_CONFIRMED_ON_DROID', 'COMPONENT_NOT_CONFIRMED_ON_DROID'].includes(confirmedWord)) {
      throw new Error(`component_registry.inc: cannot read "confirmed_on_droid" for ${match[2]}: ${confirmedWord}`);
    }
    const gate = /"(PA_CAP_\w+)"/.exec(tail);
    parts.push({
      id: match[2],
      value: Number(match[1]),
      name: match[3],
      category: categoryId.get(match[4]),
      protocol: match[5],
      status,
      confirmed_on_droid: confirmedWord === 'COMPONENT_CONFIRMED_ON_DROID',
      capabilities: 0,
      included,
      board_capability: gate ? gate[1] : null,
    });
  }
  if (parts.length < 20) throw new Error(`component_registry.inc: read only ${parts.length} parts`);
  categories.forEach((category) => {
    const selectable = parts.filter((part) => part.category === category.id && part.status === 'supported' && part.included);
    category.selectable = selectable.length;
    if (category.selectable <= 1) category.member_key = null;
    category.active_member = null;
  });
  const sound = categories.find((category) => category.id === 'sound');
  if (sound) sound.active_member = 'dy_sv5w';
  return { categories, parts };
};

// One Output row in GET /api/servo/outputs's shape (docs/api.md).
const outputRow = ({ n, id, name, wired, parts, calibrated, lightCapable, component, following = false }) => ({
  address: `ledc:${n - 1}`,
  name,
  id,
  switchable: true,
  wired,
  lightCapable,
  ledCount: lightCapable ? 16 : null,
  throwMs: 400,
  accelMs: 100,
  ease: 'none',
  release: 0,
  boot: 'limp',
  parts,
  bandLoUs: 1000,
  bandHiUs: 2000,
  component,
  openUs: calibrated ? 2000 : null,
  centreUs: calibrated ? 1500 : null,
  closeUs: calibrated ? 1000 : null,
  calibrated,
  commandedUs: null,
  targetUs: null,
  nudgesDone: 0,
  held: false,
  limp: 'off',
  // What the droid started with (#364, src/web/api_config.cpp
  // handleServoOutputsGet()): this fixture's droid started as its rows stand,
  // and a save after that leaves these four alone, which is a save waiting for
  // a restart. `driven` is the wired tick at start with no light on the wire;
  // `activeLight` is null on a servo wire; `activeLedCount` is only on a row a
  // light can go on.
  activeWired: wired,
  driven: wired && component !== 'rgb',
  activeLight: component === 'rgb' ? 'rgb' : null,
  ...(lightCapable ? { activeLedCount: 16 } : {}),
  // Not a firmware field: which row the 'bench' droid moves on every read.
  // Stripped from every answer.
  following,
});

const initialOutputs = (droid) => {
  if (droid === 'bench') {
    return [1, 2, 3, 4, 5].map((n) =>
      outputRow({
        n,
        id: `arm${n}`,
        name: `ARM${n}`,
        wired: true,
        parts: n === 1 ? ['bodyPanel1', 'bodyPanel2'] : [],
        calibrated: n === 1,
        lightCapable: false,
        component: 'none',
        following: n === 1,
      }),
    );
  }
  // Five Artoo-style Outputs (include/board_outputs.h ids and labels). ARM1
  // carries two body panels and is calibrated; AUX3 carries the CBI and the
  // LED strip Light Type, so Lights has a wire with an LED count; ARM2 is
  // wired with nothing on it; AUX1 and AUX2 are not wired.
  return [
    { n: 1, id: 'arm1', name: 'ARM1', wired: true, parts: ['bodyPanel1', 'bodyPanel2'], calibrated: true },
    { n: 2, id: 'arm2', name: 'ARM2', wired: true, parts: [], calibrated: false },
    { n: 3, id: 'aux1', name: 'AUX1', wired: false, parts: [], calibrated: false },
    { n: 4, id: 'aux2', name: 'AUX2', wired: false, parts: [], calibrated: false },
    { n: 5, id: 'aux3', name: 'AUX3', wired: true, parts: ['cbi'], calibrated: false, light: true },
  ].map((row) => outputRow({ ...row, lightCapable: row.n >= 3, component: row.light ? 'rgb' : 'mg996r' }));
};

const initialConfig = (droid) => {
  // No droidBuild key: the older-firmware answer, so the Parts picture falls
  // back to drawing every marker rather than only the fitted ones.
  if (droid === 'bench') return { system: { logLevel: 3 } };
  // The artoo droid with Dome Rotation switched on and started with, every
  // Dome Rotation Setting at its default, and Positive turns clockwise so the
  // readout names a direction.
  if (droid === 'dome') {
    const config = initialConfig('artoo');
    config.activeToggles.push('domeRotation');
    config.components.domeRotation.enabled = true;
    config.domeRotation = Object.fromEntries(DOME_ROTATION_SETTINGS.map(([, key, rule]) => [key, rule.def]));
    config.domeRotation.positiveTurn = 'cw';
    return config;
  }
  return {
    drive: { speedLimitMax: 40, speedPreset: 'normal', webDriveTimeoutMs: 500, stationary: false },
    rc: { inputMode: 'standard_pwm', activeInputMode: 'standard_pwm', sbusTimeoutMs: 300, sbus: { recvCh2: false }, member: 'rc_radio' },
    activeToggles: ['drive', 'rcCh1', 'rcCh2'],
    components: {
      domeRotation: { enabled: false, label: 'DOME' },
      rcCh1: { enabled: true, label: 'CH1' },
      rcCh2: { enabled: true, label: 'CH2' },
      rcCh3: { enabled: false, label: 'CH3' },
      rcCh4: { enabled: false, label: 'CH4' },
      rcCh5: { enabled: false, label: 'CH5' },
      rcCh6: { enabled: false, label: 'CH6' },
      drive: { enabled: true, label: 'S1' },
      audio: { enabled: false, label: 'S2', member: 'dy_sv5w', activeMember: 'dy_sv5w' },
      protoR2link: { enabled: false, label: 'S3' },
    },
    system: { logLevel: 3 },
    droidBuild: {
      domeDesign: 'mk4',
      domeVariant: 'complex',
      bodyDesign: 'mk4',
      bodyVariant: 'complex',
      fitted: ['bodyPanel1', 'bodyPanel2', 'cbi', 'dataPanel', 'pie1', 'panel1'],
    },
    // A controller configured before guided Setup existed: no record at all.
    guidedSetup: { run: 'not-run', visited: [], recorded: false, summaryDone: false },
    wifi: {
      provisioned: true,
      mode: 'client',
      staSsid: 'bench-ap',
      staPasswordSet: true,
      apSsid: 'protor2',
      apPasswordSet: false,
      pendingApply: false,
    },
  };
};

// The FIXTURE_FITTED override (header). An id the catalog (data/droid_parts.js)
// does not carry stops the script: it would otherwise run against a droid
// that fits nothing new and pass for the wrong reason.
const withFitted = (config, outputs, list) => {
  const ids = String(list || '')
    .split(',')
    .map((id) => id.trim())
    .filter(Boolean);
  if (ids.length === 0) return config;
  const catalog = { window: {} };
  vm.runInNewContext(fs.readFileSync(path.join(REPO, 'data', 'droid_parts.js'), 'utf8'), catalog);
  const known = new Set(catalog.window.DroidParts.parts.map((part) => part.id));
  const unknown = ids.filter((id) => !known.has(id));
  if (unknown.length) throw new Error(`fixture_routes: FIXTURE_FITTED names no catalog Part: ${unknown.join(', ')}`);
  // A droid with no Droid Build of its own still carries what its Outputs
  // drive, so those stay fitted and keep offering their acts.
  const build = config.droidBuild || {
    domeDesign: 'mk4',
    domeVariant: 'complex',
    bodyDesign: 'mk4',
    bodyVariant: 'complex',
    fitted: outputs.flatMap((row) => row.parts || []),
  };
  return { ...config, droidBuild: { ...build, fitted: [...new Set([...build.fitted, ...ids])] } };
};

const identityOf = (droid) => {
  if (droid === 'bench') {
    return {
      droidName: 'fixture-artoo',
      mdnsUseName: true,
      board: 'artoo_esp32',
      board_capabilities: { PA_CAP_NATIVE_WIFI: true, PA_CAP_HOSTED_WIFI: false },
      build_flags: { PA_HEAP_PROFILE: false, PA_HEAP_TRACING: false, PA_ADMISSION_TRACE: false },
    };
  }
  return {
    droidName: 'fixture-artoo',
    mdnsUseName: true,
    board: 'artoo_esp32',
    learned_sequence_cap: 5,
    learned_sequence_max_bytes: 12288,
    board_capabilities: {
      PA_CAP_NATIVE_WIFI: true,
      PA_CAP_HOSTED_WIFI: false,
      PA_CAP_DRIVE_BACKEND_HOVERBOARD: true,
      PA_CAP_DEDICATED_AUDIO_UART: false,
    },
    board_lanes: {
      drive: { uart: 1, tx: 16, rx: 17 },
      audio: { uart: 2, tx: 26, rx: 35 },
      protor2link: { uart: 2, tx: 33, rx: 34, baud: 9600, protocol: 'marcduino' },
    },
    build_flags: { PA_HEAP_PROFILE: false, PA_HEAP_TRACING: false, PA_ADMISSION_TRACE: false },
  };
};

// Enabled in ServoTask's sense: wired, and not a wire kept for a light.
const isEnabledOutput = (row) => row.wired && row.component !== 'rgb';

const parseForm = (body) => Object.fromEntries(new URLSearchParams(body || ''));

const install = async (context, options = {}) => {
  const droid = options.droid || 'artoo';
  if (!DROIDS.includes(droid)) throw new Error(`fixture_routes: no droid called "${droid}" (${DROIDS.join(', ')})`);
  const registry = readRegistry();
  const identity = identityOf(droid);

  const state = {
    droid,
    estop: options.estop === true,
    sleep: false,
    outputs: initialOutputs(droid),
    // Wall-clock of the last hold command per address, while a hold stands.
    holds: new Map(),
    // Writes refused (kept so "no writes" has something to read), and the
    // guided Setup visit record taken.
    writes: [],
    accepted: [],
    outputsReads: 0,
    // The Sequence run the droid last recorded, as the status frame carries
    // it: null until a run is recorded, else {name, running, startMs}. Every
    // frame has the key (src/web/status_json.cpp, #451). Nothing here starts a
    // run; a script's own droid that does sets it and pushes a status, as the
    // firmware asks for one when a run begins and when it ends
    // (src/sequence_run_evidence.cpp).
    seqRun: null,
    config: withFitted(initialConfig(droid), initialOutputs(droid), process.env.FIXTURE_FITTED),
    // What the dome droid's Dome Rotation last put on the wire, null on the
    // other droids: {targetSpeed, share, pulseUs}, set by domeWrite() below.
    dome: null,
  };

  // The dome droid's one output, written the way setDomeSpeed() writes it
  // (src/tasks/dome_rotation_task.cpp): the pulse from the saved Settings at
  // the moment of the write, and the share the backend reports for it. Speed
  // 0 is neutral, which is also setDomeNeutral()'s answer.
  let domeStopTimer = null;
  const domeWrite = (speed) => {
    const cfg = state.config.domeRotation;
    const pulseUs = domeSpeedToPulseUs(speed, cfg.neutralUs, cfg.minPulseUs, cfg.maxPulseUs, cfg.speedLimitPct, cfg.smallestTurnUs);
    const share = domePulseToShare(pulseUs, cfg.neutralUs, cfg.minPulseUs, cfg.maxPulseUs, cfg.smallestTurnUs);
    state.dome = { targetSpeed: speed, share, pulseUs };
  };
  // A stop, and the end of whatever timer would have stopped the turn.
  const domeStop = () => {
    clearTimeout(domeStopTimer);
    domeStopTimer = null;
    domeWrite(0);
  };
  if (droid === 'dome') domeWrite(0);

  const status = () => {
    if (droid === 'bench') {
      return {
        // The six fields data/live_reading.js requires of a frame.
        estop: state.estop,
        sbusHwFailsafe: false,
        sbusSignalLost: false,
        webDriveExpired: false,
        webControlEnabled: false,
        sleepMode: state.sleep,
        // Enough beyond them for every Status Plate chip to read a value.
        drive: { backend: 'none' },
        speedLimitMax: 40,
        rcCh1: { state: 'not_seen' },
        stationary: false,
        uptimeMs: Date.now() % 100000000,
        seqRun: state.seqRun,
      };
    }
    const frame = {
      estop: state.estop,
      sbusHwFailsafe: false,
      sbusSignalLost: false,
      webDriveExpired: false,
      webControlEnabled: false,
      sleepMode: state.sleep,
      drive: { state: 'idle', detail: 'Enabled' },
      speedLimitMax: 40,
      speedPreset: 'normal',
      stationary: false,
      activeMood: 0,
      domeEnabled: false,
      rcCh1: { state: 'ready', detail: 'Standard PWM input enabled' },
      rcCh2: { state: 'ready', detail: 'Standard PWM input enabled' },
      uptimeMs: Date.now() % 100000000,
      firmwareVersion: 'fixture',
      fsVersion: 'fixture',
      resetReason: 'POWERON',
      heapFree: 150000,
      heapMin: 120000,
      heapLargestBlock: 90000,
      wifiConnected: true,
      wifiClientConnected: true,
      wifiRssi: -55,
      littleFsReady: true,
      dome_link: { state: 'disabled' },
      lights: {},
      seqRun: state.seqRun,
    };
    // An enabled Output is a key of its own (src/web/status_json.cpp), and a
    // lit wire reports under its id in `lights`.
    state.outputs.forEach((row) => {
      if (isEnabledOutput(row)) frame[row.id] = { state: 'ready', detail: 'Servo channel enabled' };
      if (row.wired && row.component === 'rgb') frame.lights[row.id] = { r: 0, g: 0, b: 0, effect: 'off', available: true };
    });
    // The dome droid's Dome Rotation, as src/web/status_json.cpp writes it:
    // the sent share at one decimal, the pulse, and the entry whose token is
    // `idle` on a share of exactly 0 and whose detail is the same figure.
    if (state.dome) {
      const { targetSpeed, share, pulseUs } = state.dome;
      const sentPct = Number(Math.fround(share * 100).toFixed(1));
      Object.assign(frame, {
        domeTargetSpeed: Number(targetSpeed.toFixed(3)),
        domeRotationSentPct: sentPct,
        domeRotationPulseUs: pulseUs,
        domeEnabled: true,
        domeBearing: 'unknown',
        domeBearingDeg: null,
      });
      frame.domeRotation = {
        state: share === 0 ? 'idle' : 'spinning',
        detail: `${sentPct.toFixed(1)}%${pulseUs > 0 ? `, ${pulseUs} µs` : ''}`,
      };
    }
    return frame;
  };

  // The Outputs answer. A following row that the droid still drives moves to
  // its other width on each read.
  const outputsAnswer = () => {
    state.outputsReads += 1;
    state.outputs.forEach((row) => {
      if (!row.following || row.limp !== 'off') return;
      row.commandedUs = state.outputsReads % 2 === 0 ? 1400 : 1600;
      row.targetUs = row.commandedUs;
    });
    return {
      outputs: state.outputs.map(({ following, ...row }) => ({ ...row, parts: row.parts.slice() })),
    };
  };

  const releaseAll = (reason) => {
    state.outputs.forEach((row) => {
      if (!isEnabledOutput(row)) return;
      state.holds.delete(row.address);
      row.held = false;
      row.commandedUs = null;
      row.targetUs = null;
      row.limp = reason;
    });
  };

  // The SSE stand-in. Every open client is pushed each change, and a client
  // arriving is itself a change (src/web/api_events.cpp).
  const clients = new Set();
  const frameText = () => `event: status\ndata: ${JSON.stringify(status())}\n\n`;
  const push = () => clients.forEach((res) => res.write(frameText()));

  // The latch and its release, from a page or from outside the browser
  // (header). Each pushes a status on an actual edge only.
  const latch = () => {
    const was = state.estop;
    state.estop = true;
    releaseAll('estop');
    // DomeRotationTask puts neutral out under the estop.
    if (state.dome) domeStop();
    if (!was) push();
  };
  const unlatch = () => {
    const was = state.estop;
    state.estop = false;
    if (was) push();
  };

  const server = http.createServer((req, res) => {
    // The latch or its release from outside the browser (header). Answered
    // the way src/web/api_estop.cpp answers them.
    const estopRoute = req.url.split('?')[0];
    if (req.method === 'POST' && (estopRoute === '/api/estop' || estopRoute === '/api/estop/clear')) {
      req.resume();
      if (estopRoute === '/api/estop') latch();
      else unlatch();
      res.writeHead(200, { 'Content-Type': 'application/json' });
      res.end('{"ok":true}');
      return;
    }
    if (!req.url.startsWith('/events')) {
      res.writeHead(404);
      res.end();
      return;
    }
    res.writeHead(200, {
      'Content-Type': 'text/event-stream',
      'Cache-Control': 'no-store',
      Connection: 'keep-alive',
      'Access-Control-Allow-Origin': '*',
    });
    clients.add(res);
    push();
    const keepalive = setInterval(() => res.write(': keepalive\n\n'), 5000);
    req.on('close', () => {
      clearInterval(keepalive);
      clients.delete(res);
    });
  });
  await new Promise((resolve) => server.listen(0, '127.0.0.1', resolve));
  const ssePort = server.address().port;

  // The hold's expiry, checked on a clock the way ServoTask checks it each tick.
  const expiryTimer = setInterval(() => {
    const now = Date.now();
    state.holds.forEach((lastAt, address) => {
      if (now - lastAt <= HOLD_EXPIRY_MS) return;
      state.holds.delete(address);
      const row = state.outputs.find((each) => each.address === address);
      if (!row) return;
      row.held = false;
      row.commandedUs = null;
      row.targetUs = null;
      row.limp = 'expiry';
    });
  }, 100);

  // A droid that starts latched has already been through the latch's edge.
  if (state.estop) releaseAll('estop');

  const json = (route, body, code = 200) =>
    route.fulfill({ status: code, contentType: 'application/json', body: JSON.stringify(body) });

  const refuse = (route, method, apiPath, body) => {
    // A write this fixture was never meant to see. Refused, and kept, so a
    // script that asserts "no writes" has something to read.
    state.writes.push(`${method} ${apiPath} ${String(body || '').slice(0, 80)}`.trim());
    return json(route, { ok: false, error: 'fixture refuses writes' }, 400);
  };

  // POST /api/sleep and /api/wake (header).
  const setSleep = (route, sleeping) => {
    const changed = state.sleep !== sleeping;
    state.sleep = sleeping;
    // DomeRotationTask puts neutral out in Sleep Mode.
    if (sleeping && state.dome) domeStop();
    if (changed) push();
    return json(route, { ok: true, sleepMode: sleeping, changed });
  };

  // GET /api/rc for a droid with no receiver switched on (header).
  const rcDiagnostics = () => ({
    mode: state.config.rc?.inputMode || 'standard_pwm',
    updatedMs: Date.now() % 100000000,
    sources: {
      sbus1: { enabled: false, linked: false, ageMs: 0, lostFrames: 0, failsafe: false },
      sbus2: { enabled: false, linked: false, ageMs: 0, lostFrames: 0, failsafe: false },
      pwm: { enabled: false, linked: false, ageMs: 0, lostFrames: 0, failsafe: false },
    },
    channels: [],
    digital: {},
    mappingProfile: { channels: [] },
    raw: {},
  });

  const servoPost = (route, body) => {
    const form = parseForm(body);
    const row = state.outputs.find((each) => each.name === form.arm);
    if (!row) return json(route, { ok: false, error: `No output called ${form.arm} on this board` }, 400);
    if (form.action === 'hold') {
      const us = Number(form.positionUs);
      if (!Number.isFinite(us) || us < 500 || us > 2500) return json(route, { ok: false, error: 'positionUs must be between 500 and 2500' }, 400);
      // Queued and answered; ServoTask refuses every command under the latch.
      if (state.estop) return json(route, { ok: true });
      const refresh = form.refresh === '1';
      if (refresh && !state.holds.has(row.address)) return json(route, { ok: true });
      state.holds.set(row.address, Date.now());
      row.held = true;
      row.commandedUs = us;
      row.targetUs = us;
      return json(route, { ok: true });
    }
    if (form.action === 'release') {
      if (state.estop) return json(route, { ok: true });
      state.holds.delete(row.address);
      row.held = false;
      row.commandedUs = null;
      row.targetUs = null;
      row.limp = 'pulses-off';
      return json(route, { ok: true });
    }
    return refuse(route, 'POST', '/api/servo', body);
  };

  // POST /api/dome on the dome droid (header), refused in the words
  // handleDomeSpeedPost() uses.
  const domePost = (route, body) => {
    const form = parseForm(body);
    if (!('speed' in form)) return json(route, { ok: false, error: 'missing speed' }, 400);
    // strtof() over the whole value: a decimal number and nothing after it.
    const speedText = form.speed;
    const speed = /^\s*[+-]?(\d+\.?\d*|\.\d+)([eE][+-]?\d+)?$/.test(speedText) ? Math.fround(Number(speedText)) : NaN;
    if (!(speed >= -1 && speed <= 1)) return json(route, { ok: false, error: 'speed must be a float in range -1.0..1.0' }, 400);
    let durationMs = 0;
    if ('durationMs' in form) {
      durationMs = /^\d+$/.test(form.durationMs) ? Number(form.durationMs) : NaN;
      if (!(durationMs <= DOME_TIMED_TURN_MAX_MS)) {
        return json(route, { ok: false, error: 'durationMs must be an integer in range 0..30000' }, 400);
      }
    }
    if (state.sleep) return json(route, { ok: false, error: 'sleeping', hint: 'POST /api/wake' }, 423);
    if (!state.config.activeToggles.includes('domeRotation')) {
      return json(route, { ok: false, error: 'dome output is disabled' }, 409);
    }
    // Queued, and discarded by the task while the estop is latched.
    if (state.estop) return json(route, { ok: true });
    // A command replaces the one before it, and its own end replaces that
    // one's: a timed turn ends after its time, an untimed one when nothing
    // has followed it for the silence stop.
    clearTimeout(domeStopTimer);
    domeWrite(speed);
    domeStopTimer = setTimeout(() => {
      domeStop();
      push();
    }, durationMs > 0 ? durationMs : DOME_COMMAND_TIMEOUT_MS);
    push();
    return json(route, { ok: true });
  };

  // The Dome Rotation Settings on the dome droid (header): every field of the
  // form one of them, or this is not a write this droid takes (null). Judged
  // in the settings table's order and merged over what is saved, as
  // configApply() judges a request, so the first refusal is the one the droid
  // gives and nothing is saved beside it.
  const domeConfigPost = (route, body) => {
    const form = parseForm(body);
    const sent = DOME_ROTATION_SETTINGS.filter(([name]) => name in form);
    if (sent.length === 0 || sent.length !== Object.keys(form).length) return null;
    const next = { ...state.config.domeRotation };
    for (const [name, key, rule] of sent) {
      const raw = form[name];
      if (rule.bool) {
        if (!['true', 'false', '1', '0'].includes(raw)) {
          return json(route, { ok: false, error: `${name} must be true/false or 1/0`, field: name, reason: 'out-of-range', accepts: 'true,false,1,0' }, 400);
        }
        next[key] = raw === 'true' || raw === '1';
      } else if (rule.words) {
        if (!rule.words.includes(raw)) {
          const sentence = `${rule.words.slice(0, -1).join(', ')} or ${rule.words[rule.words.length - 1]}`;
          return json(route, { ok: false, error: `${name} must be ${sentence}`, field: name, reason: 'out-of-range', accepts: rule.words.join(',') }, 400);
        }
        next[key] = raw;
      } else {
        const value = /^\d+$/.test(raw) ? Number(raw) : NaN;
        if (!(value >= rule.lo && value <= rule.hi)) {
          return json(route, { ok: false, error: `${name} must be ${rule.lo}..${rule.hi}`, field: name, reason: 'out-of-range', accepts: `${rule.lo}..${rule.hi}` }, 400);
        }
        next[key] = value;
      }
    }
    // The two rules across Settings, only when the request named one of
    // theirs (src/web/api_config_apply.cpp): the pulses in order, and the
    // random pauses' shortest no longer than their longest.
    const firstSent = (names) => names.find((name) => name in form);
    const pulseSent = firstSent(['domeRotationNeutralUs', 'domeRotationMinPulseUs', 'domeRotationMaxPulseUs']);
    if (pulseSent && !(next.minPulseUs <= next.neutralUs && next.neutralUs <= next.maxPulseUs)) {
      return json(route, {
        ok: false,
        error: `domeRotationMinPulseUs ${next.minPulseUs}, domeRotationNeutralUs ${next.neutralUs}, domeRotationMaxPulseUs ${next.maxPulseUs}: must be min <= neutral <= max`,
        field: pulseSent,
        reason: 'conflict',
      }, 400);
    }
    const pauseSent = firstSent(['domeRotationRndPauseMin', 'domeRotationRndPauseMax']);
    if (pauseSent && next.rndPauseMin > next.rndPauseMax) {
      return json(route, { ok: false, error: 'domeRotationRndPauseMin must be at most domeRotationRndPauseMax', field: pauseSent, reason: 'conflict' }, 400);
    }
    state.config.domeRotation = next;
    state.accepted.push(`POST /api/config ${body}`);
    return json(route, state.config);
  };

  const configPost = (route, request, body) => {
    // Guided Setup's visit record (data/setup.js saveVisited): taken, and kept.
    if (/^guidedSetupVisited=[^&]*$/.test(body)) {
      state.accepted.push(`POST /api/config ${body}`);
      return json(route, { ok: true });
    }
    const type = request.headers()['content-type'] || '';
    if (state.dome && !type.includes('application/json')) {
      const answered = domeConfigPost(route, body);
      if (answered) return answered;
    }
    if (!type.includes('application/json')) return refuse(route, 'POST', '/api/config', body);
    let parsed;
    try {
      parsed = JSON.parse(body || '');
    } catch (_error) {
      return json(route, { ok: false, error: 'invalid JSON' }, 400);
    }
    const keys = Object.keys(parsed || {});
    if (keys.length !== 1 || keys[0] !== 'outputs' || !Array.isArray(parsed.outputs)) {
      return refuse(route, 'POST', '/api/config', body);
    }
    for (const sent of parsed.outputs) {
      const row = state.outputs.find((each) => each.address === sent.address);
      if (!row) return json(route, { ok: false, error: `${sent.address} is not an Output`, field: `${sent.address}` }, 400);
      if ('ledCount' in sent) {
        if (!row.lightCapable || !Number.isInteger(sent.ledCount) || sent.ledCount < 1 || sent.ledCount > 255) {
          return json(route, { ok: false, error: 'ledCount must be between 1 and 255', field: `${sent.address}.ledCount` }, 400);
        }
        row.ledCount = sent.ledCount;
      }
      // The Output Release time, 0 (never) to a minute, as the row door takes it (#443).
      if ('release' in sent) {
        if (!Number.isInteger(sent.release) || sent.release < 0 || sent.release > 60000) {
          return json(route, { ok: false, error: 'release must be 0..60000 ms', field: `${sent.address}.release` }, 400);
        }
        row.release = sent.release;
      }
    }
    state.writes.push(`POST /api/config ${body.slice(0, 80)}`);
    return json(route, state.config);
  };

  // The pictures: image/webp for every safely named file the set carries, as
  // the device answers them since #355 finding 4.
  const photoHandler = async (route) => {
    const url = new URL(route.request().url());
    const name = path.basename(url.pathname, '.webp');
    const file = path.join(ASSETS, `${name}.webp`);
    if (!fs.existsSync(file)) return route.fulfill({ status: 404, contentType: 'text/plain', body: 'Not found' });
    const body = fs.readFileSync(file);
    // `textPlainPhotos` is a self-test's: pictures answered the wrong way.
    const safeShape = /^[a-z][a-z0-9_]*$/.test(name);
    const typed = safeShape && !(options.textPlainPhotos || []).includes(name);
    return route.fulfill({ status: 200, contentType: typed ? 'image/webp' : 'text/plain', body });
  };

  const apiHandler = async (route) => {
    const request = route.request();
    const url = request.url();
    const apiPath = url.slice(url.indexOf('/api/')).split('?')[0];
    const method = request.method();
    const body = request.postData() || '';

    if (apiPath === '/api/events') {
      await route.continue({ url: `http://127.0.0.1:${ssePort}/events` });
      return;
    }
    if (method === 'GET') {
      if (apiPath === '/api/identity') return json(route, identity);
      if (apiPath === '/api/identity/components') return json(route, registry);
      if (apiPath === '/api/status') return json(route, status());
      if (apiPath === '/api/config') return json(route, state.config);
      if (apiPath === '/api/servo/outputs') return json(route, outputsAnswer());
      if (apiPath === '/api/rc') return json(route, rcDiagnostics());
      if (apiPath === '/api/rc/map') {
        // `receivers` as the droid answers them for its saved type (ADR 0070).
        const mode = state.config.rc?.inputMode || 'standard_pwm';
        const receivers = {
          dual_sbus: { read: ['sbus1', 'sbus2'], drive: ['sbus1'], cues: ['sbus1', 'sbus2'] },
          single_sbus: { read: ['sbus1'], drive: ['sbus1'], cues: ['sbus1'] },
          standard_pwm: { read: ['pwm'], drive: ['pwm'], cues: [] },
        }[mode] || { read: [], drive: [], cues: [] };
        return json(route, { mode, map: [], capacity: { total: 14, used: 0 }, receivers });
      }
      if (apiPath === '/api/seq/list' || apiPath === '/api/seq/builtins') return json(route, []);
      // Every other read goes to the fixture server, which 404s it the way
      // the controller answers a route it does not have.
      return route.fallback();
    }
    if (method === 'POST' && apiPath === '/api/estop') {
      latch();
      return json(route, { ok: true });
    }
    if (method === 'POST' && apiPath === '/api/estop/clear') {
      unlatch();
      return json(route, { ok: true });
    }
    if (method === 'POST' && apiPath === '/api/sleep') return setSleep(route, true);
    if (method === 'POST' && apiPath === '/api/wake') return setSleep(route, false);
    if (method === 'POST' && apiPath === '/api/servo') return servoPost(route, body);
    if (method === 'POST' && apiPath === '/api/config') return configPost(route, request, body);
    if (method === 'POST' && apiPath === '/api/dome' && state.dome) return domePost(route, body);
    return refuse(route, method, apiPath, body);
  };

  const routeContext = async (target) => {
    await target.route('**/*.webp*', photoHandler);
    await target.route('**/api/**', apiHandler);
  };
  await routeContext(context);

  return {
    state,
    ssePort,
    // Routes another browser context against this same droid (header).
    addContext: routeContext,
    // Sends every open stream the current status, as a state change on the
    // droid would; a self-test that changes `state` calls it.
    push,
    // The operator releasing the latch with STOP in another window, for a
    // script that needs it released and does not press STOP itself.
    clearEstop: () => {
      state.estop = false;
      push();
    },
    close: () =>
      new Promise((resolve) => {
        clearInterval(expiryTimer);
        clearTimeout(domeStopTimer);
        clients.forEach((res) => res.end());
        server.close(() => resolve());
        // A browser that is gone may leave its socket half-open; do not let
        // it hold the script's exit.
        server.closeAllConnections();
      }),
  };
};

module.exports = { install, DROIDS };
