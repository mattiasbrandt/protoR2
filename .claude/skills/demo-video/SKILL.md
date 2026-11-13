---
name: demo-video
description: Recreate protoR2's project demo video and README GIF - scripted takes of the web UI on a real board, recorded and cut with OpenScreen's MCP tools, titles composited with ffmpeg. Use when asked to remake, update or extend the demo video or showcase GIF, or to add a page or feature to it.
---

# protoR2 demo video

The demo is a **scripted take**: `drive.js` plays the web UI on a real board in a
clean app window, OpenScreen records and cuts it, and `overlay/` draws the titles
that ffmpeg lays on top. A remake is the same pipeline with new beats, so every
run gives the same video for the same UI.

First made 2026-11-13 (issue #509): 89.5 s MP4, 1920x1080, 60 fps, 21 MB, and a
24.7 s README GIF, 852x480, 10 MB. Status and the hosted video are on #509.

## What you need

- OpenScreen's MCP tools (`startRecording`, `stopRecording`, `exportVideo`,
  `getFrames`). Missing: follow `.agents/skills/openscreen-demo/SKILL.md` step 1,
  which ends in a new session.
- A board on the network in Bench-Mode. Read `GET /api/identity`,
  `/api/status` and `/api/seq/builtins` first, and never flash it for this.
- Playwright (`NODE_PATH=$HOME/.npm/_npx/e41f203b7505f1fb/node_modules`), ffmpeg
  with libx264, and `overlay/fetch_fonts.sh` run once.
- The operator at the screen for each take: the portal picker asks him for the
  monitor, and he stays on the take's workspace, hands off the mouse.

## This machine (Hyprland), measured

- **`cursor: "system"` on every `startRecording`.** The default mode never
  starts: the portal has no METADATA cursor mode. So there is no pointer track
  and no click zoom; the page draws its own pointer and zooms are placed by hand.
- **A take is the whole monitor of the visible workspace**, 3440x1440. Bring the
  take's workspace forward after the picker and before `go`; crop the clip to
  the window rect in the log.
- **`listSources` times out** until a picker is answered. Skip it.
- `hyprctl dispatch` takes Lua (`hl.dsp.*`); `drive.js` has the calls.
- OpenScreen's text has one font, so its text annotations are not used. Its
  blur annotations are.

## Steps

### 1. Dry run

    DRY=1 BEAT=all node .claude/skills/demo-video/drive.js
    DRY=1 BEAT=7   node .claude/skills/demo-video/drive.js

Headless, no window, no take. Done when every beat prints `played`, the log
lists no refused write you did not expect, and each
`tasks/demo-video/shots/dry-<n>.png` shows what the beat is for. `BASE_URL` is
the board (default `http://firebeetle2.local`).

### 2. Takes

Two takes, each the same way. Tell the operator before each one.

1. Start `BEAT=all` (or `BEAT=7`) in the background, output to a **new** file,
   and wait for `Waiting for go`. A reused output file carries the last run's
   error and reads as a failure.
2. `startRecording` with `cursor: "system"`. It returns when he has picked.
3. Focus workspace 9 and the window, park the real pointer left of the window
   (`hl.dsp.cursor.move({ x = 300, y = 760 })`), then `touch tasks/demo-video/go`.
4. Wait for `Waiting for done` in **that** output file, then `stopRecording`,
   then `touch done`.

Done when `getFrames` on each `videoPath` shows the app window, the right
workspace, and every beat. A wrong take is recorded again, not fixed in the edit.

Beat 7 sets the board's log level to Debug. Leave it and say so.

### 3. Edit in OpenScreen

Work in the `BEAT=all` take's project; `importMedia` the console take and
`insertClip` it last. Times come from `log-<BEAT>.json`: `marks` are each beat's
start and end, `events` the pointer actions, both on `recordingStartedAt`'s
clock, which matches the video's own seconds.

- **Crop** every clip to the window: from the log's window `at`/`size` over the
  monitor, inset 3 px for the border.
- **Look**: `aspectRatio: "16:9"`, `formatFollowCursor: false`, `padding: 30`,
  `cursor.show: false`, wallpaper
  `linear-gradient(135deg, #0b1220 0%, #12305c 55%, #1d4ed8 100%)`.
- **Trims**: the pre-roll, each gap between two beats' marks plus the beat's
  first 1.5 s of waiting, and the tail. A trim on the second clip needs its
  `assetId` as well as its `clipId`.
- **Zooms**, one per result, held 1.5 s or more, clear of every trim by the
  0.8 s the camera takes to move. What worked: 1.5x on the droid picture, the
  wires, the timeline and the tempo; 1.25x across the Configuration walk, the
  Parts list and the Rehearsal.
- **Blur what is private.** The Console's boot log shows the WiFi name and the
  IP, and the lines climb as new ones arrive. A blur's box is percent of the
  **cropped** clip, not of the take. Find each line's position per moment by
  matching a strip of the take frame by frame, then cover each run of positions
  with one box. Check every step with `getFrames`.

Done when `getFrames` across the ruler shows each beat's result in frame, no
private text, and no jump at a cut.

### 4. Titles and export

1. `exportVideo` the project with no text annotations to
   `~/Videos/protoR2-demo/work/protoR2-clean.mp4` (`quality: "good"`, 60 fps).
2. Set the cues in `overlay/overlay.html` (`CUES`, `END`) on the **export's**
   clock: ruler seconds minus every trim before them. `END.total` is the clean
   export's length plus the held end card.
3. `AT=<seconds> node overlay/render.js <dir>` for a few stills, laid over the
   matching video frames, before the full render.
4. Render and composite:

       node .claude/skills/demo-video/overlay/render.js $W/frames
       ffmpeg -y -i $W/protoR2-clean.mp4 -framerate 30 -i $W/frames/f_%05d.png \
         -filter_complex "[0:v]tpad=stop_mode=clone:stop_duration=4[v];[v][1:v]overlay=eof_action=repeat,trim=duration=<END.total>,format=yuv420p[o]" \
         -map "[o]" -an -c:v libx264 -preset medium -crf 18 -r 60 -movflags +faststart \
         ~/Videos/protoR2-demo/protoR2-launch.mp4

5. The GIF is cut from that MP4: `trim` three beats and the end card, `concat`,
   `fps=15,scale=852:480`, `palettegen=max_colors=200` and
   `paletteuse=dither=bayer`. Keep it under the 40 MB cap from #480.

Done when a contact sheet of the MP4 shows every title readable and clear of
the action, the end card complete on the last frame, and both files' sizes and
lengths are reported. The operator watches both before anything is posted or
committed. The video never enters git; the GIF only on the branch that changes
the README.

## Splash, logo and the README

The end card is also the project's logo. `overlay/splash.js <dir>` renders it on
its own ground: the splash's frames, `protoR2-logo.png` and
`protoR2-wordmark.png` (transparent, for a dark ground). The loop is

    ffmpeg -y -framerate 20 -i <dir>/splash/f_%04d.png -filter_complex \
      "fade=t=out:st=6.0:d=0.5:c=0x050a16,scale=960:540:flags=lanczos,split[s0][s1];[s0]palettegen=max_colors=128:stats_mode=diff[p];[s1][p]paletteuse=dither=bayer:bayer_scale=5:diff_mode=rectangle" \
      -loop 0 docs/images/brand/protoR2-splash.gif

`docs/images/brand/` holds those three and `protoR2-demo.gif`, the README GIF.
The README opens with the logo, then the GIF as a link to the MP4. The MP4 is
hosted by GitHub, not git: `gh issue comment 509 --body-file <file> --attach
./protoR2-launch.mp4` with `![](./protoR2-launch.mp4)` in the body, then read
the comment back for its `user-attachments` address and put that in the README.
Uploading publishes it, so the operator says yes first.

The end card carries no motto (operator, 2026-11-13): a line about the browser
read as web control only, and RC and driving are equal uses.

## Adding a beat

A beat is an entry in `BEATS` in `drive.js` (`stage` before the take, `play`
during it) and a cue in `overlay.html`. Take selectors and waits from the
surface's script under `test/playwright/`, and try it with `DRY=1` first.

- Start each `play` with a 2.8 s wait, and pause a second after each result.
- A beat that must write gets its own take and its own allow-list in the route
  handler, as beat 7 has. Everything else stays refused.
- Bench means the board alone: no beat claims a servo moved or a light lit.
- Titles are six words or fewer, in GLOSSARY.md's terms.

Not in the video yet, wanted by the operator (2026-11-13): Servos, Dome
Rotation, Lights, RC Control.
