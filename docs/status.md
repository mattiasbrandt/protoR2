# Project Status

protoR2 is open-source firmware for MK4 astromech droids. It runs on the
controller in the droid's body. This page says what's ready and what isn't yet.

To get started, see the [README](../README.md). For every change, see the
[changelog](../CHANGELOG.md).

## At a Glance

| | |
|---|---|
| Latest release | [Releases](https://github.com/mattiasbrandt/protoR2/releases/latest) |
| Newest firmware to download | [`v1.5.0`](https://github.com/mattiasbrandt/protoR2/releases/tag/v1.5.0) (2026-10-10). Fix releases come without downloads, so fixes since then arrive with the next feature release |
| Body Controllers | Artoo PCB; FireBeetle 2 (read ["Before you buy one"](spec-sheets/firebeetle2-esp32-p4-spec-sheet.md#before-you-buy-one) first) |
| Which parts work | [Parts](#parts), below |

## Parts

Pick what you own: the parts in each family are peers. **Supported** means
protoR2 works with the part. **Tested** means it has run on a real droid.

### Supported today

| Family | Part | Tested | Good to know |
|---|---|:---:|---|
| Body Controller | Artoo PCB | Yes | Carries a generic ESP32 board |
| | [FireBeetle 2 (ESP32-P4)](spec-sheets/firebeetle2-esp32-p4-spec-sheet.md) | | Read "Before you buy one" first |
| Radio Controller | [HotRC DS-650](spec-sheets/hotrc-ds650-radio.md) | Yes | Run with HotRC SBUS-A receivers. Its bundled receiver works as PWM |
| | RC Radio | | Any other radio, through a receiver below |
| | [RC Receiver - PWM](spec-sheets/rc-receiver-spec.md) | | |
| | [RC Receiver - SBUS](spec-sheets/sbus-protocol.md) | | |
| Body servos | Board outputs | Yes | The controller's own outputs, for a servo or a light strip |
| | [PCA9685](spec-sheets/pca9685-servo-expander.md) | | 16 more channels on one small board |
| Dome Rotation | [ISDT ESC70](spec-sheets/isdt-esc70-dome-esc.md) | Yes | Needs tuning to your dome |
| Dome Controller | [AstroPixels Plus](spec-sheets/astropixels-dome-controller.md) | Yes | Runs the [protoR2 version](https://github.com/mattiasbrandt/AstroPixelsPlus) of its firmware. Other versions get basic commands at best |
| Foot Drive | [Hoverboard, hacked firmware](spec-sheets/hoverboard-hacked-firmware-foot-drive.md) | | You build and flash the hoverboard's firmware yourself |
| Sound | [CHIRP Audio Trigger](spec-sheets/chirp-audio-trigger-sound.md) | Yes | A stock module needs its speed set to 9600 |
| | [DY-SV5W](spec-sheets/dy-sv5w-sound.md) | Yes | |
| | [MP3 Trigger](spec-sheets/mp3-trigger-sound.md) | | Needs its speed set to 9600 and a blank track on the card |

You can also drive from a browser with no radio at all. Pick your sound module on
the droid, with no new firmware. It takes effect at the next start.

### On the roadmap

Planned, with no dates and in no set order. None of these work yet.

| Family | Part | Where it stands |
|---|---|---|
| Radio Controller | [RC Receiver - ELRS](spec-sheets/elrs-crsf-radio.md) | Researched |
| | [Xbox Controller](spec-sheets/xbox-controller-input.md) | How it connects depends on the board: USB dongle, wired or Bluetooth |
| Body servos | [Pololu Maestro](spec-sheets/pololu-maestro-servo-controller.md) | A 6 to 24 channel servo board, researched |
| Dome Rotation | [SyRen 10](spec-sheets/sabertooth-syren-packet-serial.md) | In R/C mode it may need no new firmware. Not tried yet |
| Dome Controller | [Teeces](spec-sheets/teeces-dome-lighting.md) | Researched |
| Foot Drive | [Sabertooth 2x25](spec-sheets/sabertooth-syren-packet-serial.md) | Discontinued, and its maker now points to the 2x32. Which to support is still open |
| | [Flipsky Mini V6 VESC](spec-sheets/flipsky-vesc-foot-drive.md) | Researched, and how to connect it is still to be decided |
| Sound | [DFPlayer Mini](spec-sheets/dfplayer-mini-sound.md) | Researched |

## What Works

### With the real parts connected

Most of this was checked at `v1.0.0`. Later releases changed a lot, and not all
of it has been run again.

- **Sound:** playback, named sounds, random chatter, boot sounds and sleep on the
  CHIRP Audio Trigger. Playback and volume on the DY-SV5W.
- **Radio:** setup, channel mapping, and sticks and switches firing sounds and
  dome moves.
- **Dome:** rotation, panel sequences, lights and the link to the dome.
- **Servos:** arms and other servos.
- **Stops:** emergency stop, and the stop when the radio signal drops. Not with
  Foot Drive running.

### On the controller, with nothing else attached

- **Web pages:** the pages since `v1.4.0`, gone through by hand on a FireBeetle 2,
  with automated runs on both boards.
- **Backups:** saving and restoring a droid's settings.
- **WiFi setup:** joining a home network or using the droid's own hotspot, from
  the browser. Settings survive restarts and updates.
- **Firmware updates:** over WiFi and over USB.
- **Console:** typing commands on both boards, in the browser and over a cable. It
  still answers when the web pages have gone quiet.
- **Long sessions:** hours of constant web use without a restart, at `v1.1.0`,
  before the pages were rebuilt.
- **FireBeetle 2 WiFi:** restarting the WiFi module on purpose. It comes back on
  its own and the droid stays up.

## Still to Try

- **Foot Drive** on a finished droid, with a hoverboard turning wheels.
- **The FireBeetle 2** with parts wired to it.
- **MP3 Trigger** on a droid.
- **Newer moves on real parts:** the `v1.4.0` servo pace, light strips and wiring
  sheet, and losing the radio signal with a live receiver.
- **A fresh start** from a downloaded release on a new controller, and a real
  interrupted update.
- **Moving settings** from an Artoo PCB to a FireBeetle 2.

## Release History

| Release | What it added |
|---|---|
| `v0.1.0` | Foot Drive, radio control, stopping on signal loss |
| `v0.2.0` | WiFi, web pages, firmware updates over WiFi |
| `v0.3.0` | Arm servos, dome motor, radio channel setup |
| `v0.4.0` | Sound, the two-way link to the dome |
| `v1.0.0` | First stable release: reliable pages, WiFi setup from the browser, ready-to-flash downloads |
| `v1.1.0` | The FireBeetle 2 joins as a Body Controller |
| `v1.2.0` | The Console: type anything the droid can do, in the browser or over a cable |
| `v1.4.0` | The modular release: the project becomes protoR2, with rebuilt pages, Sequences on a timeline, Wiring, guided Setup and a lineup of parts to pick from |
| `v1.5.0` | The RC page says why the droid won't save, and shows what the droid reads as pressed |
