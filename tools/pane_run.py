#!/usr/bin/env python3
"""Run one command in a fresh Herdr pane, wait for it, and say how it ended.

Herdr is two products, and this runs under either. The Herdr TUI (`herdr`,
the older multiplexer) sets HERDR_ENV=1: the command gets a split pane beside
the caller's. The Herdr app (`herdr-app` and herdr-core's `hc`) sets
HC_TERMINAL_ID, the caller's own terminal: the command gets an `hc` terminal
of its own, given a pane beside the caller's with `herdr-app do
terminal.keep`. Either way the operator can watch it and the log is verified.

WHY THIS EXISTS
---------------
A device or gate session used to be four hand-typed steps, every time: split a
pane, `herdr pane run` the command under tools/gate_in_pane.sh, poll the log in
a `for i in $(seq ...)` loop for GATE_EXIT=, then grep the lines that mattered
out of the log. One epic day wrote that sequence about 40 times. This is the
sequence, once:

    python3 tools/pane_run.py /tmp/build.log -- make build BUILD_ENV=artoo_esp32
    python3 tools/pane_run.py /tmp/flash.log --script /tmp/flash.sh --grep 'error|GATE'

It splits a sibling pane next to the caller's, runs the command there under
gate_in_pane.sh (the operator can watch the pane; the log is what is
verified), waits for the log's GATE_EXIT=<n> line, prints the tail, closes the
pane it made, and exits <n>. It never closes any other pane.

--script exists for the upload guard: it matches "make flash" or "make ota"
anywhere in a Bash command, so an upload has to live in a file (written with
the Write tool) and reach the pane by path. `--script FILE` runs `bash FILE`.

The pane's working directory is --cwd, default the caller's: a tool that writes
a run directory relative to its cwd writes it where you ran this, not wherever
a reused pane happened to be.

Herdr facts this relies on, read 2026-10-05: `herdr pane read` can return
nothing while a command is running, and `pane wait-output` matches the command
text itself when a sentinel appears in it - so the log is polled, never the
pane.

One log, one run. Two invocations naming the same log would otherwise read
each other's GATE_EXIT and close their own pane on someone else's verdict -
mid-build or mid-upload. So a run holds an exclusive lock on <log>.pane-run.lock
for its whole wait, refuses a log some other run is still writing (one with no
GATE_EXIT yet), and stamps its own PANE_RUN_ID=<id> as the log's first line:
a GATE_EXIT under any other first line is not this run's. Prefer a log name
that says whose run it is (/tmp/gate-<slug>.log) over a shared /tmp/gate.log.

Herdr app facts this relies on, read 2026-11-03 (hc 0.1.17): `terminal.keep`
takes the terminal's address (t7), not its name; a program that exits closes
its own pane; `hc rm` removes the finished terminal. --no-notify keeps hc
from typing notes about the run into the caller's terminal, which is an agent's
prompt. --keep leaves the finished terminal for `hc read`; its pane has already
closed with the program.

Exit codes: the command's own; 2 for usage or a log another run owns; 3 when
neither Herdr is available; 124 when --timeout passes (the pane is left
running and named).
"""

from __future__ import annotations

import argparse
import fcntl
import json
import os
import re
import shlex
import subprocess
import sys
import time
import uuid
from pathlib import Path

GATE_IN_PANE = Path(__file__).resolve().parent / "gate_in_pane.sh"
EXIT_RE = re.compile(r"^GATE_EXIT=(\d+)$")
RUN_ID_PREFIX = "PANE_RUN_ID="
EXIT_USAGE = 2
EXIT_NO_HERDR = 3
EXIT_TIMEOUT = 124
APP_RETRIES = 20
APP_RETRY_S = 0.25


def note(message: str) -> None:
    print(f"[pane-run] {message}", file=sys.stderr, flush=True)


def herdr(*args: str) -> dict:
    return run_json("herdr", *args)


def gate_exit(log: Path, run_id: str | None = None) -> int | None:
    """The GATE_EXIT code once gate_in_pane.sh has written its last line, else None.

    With `run_id`, only a log whose first line is this run's stamp counts: a
    finished log some other run wrote at the same path is not this one ending.
    """
    try:
        lines = log.read_text(errors="replace").splitlines()
    except OSError:
        return None
    if run_id is not None and (not lines or lines[0].strip() != f"{RUN_ID_PREFIX}{run_id}"):
        return None
    for line in reversed(lines):
        if line.strip():
            match = EXIT_RE.match(line.strip())
            return int(match.group(1)) if match else None
    return None


def summary(log: Path, tail: int, pattern: str | None) -> list[str]:
    """The last `tail` lines of the log, or of the lines matching `pattern`."""
    lines = [l for l in log.read_text(errors="replace").splitlines()
             if not EXIT_RE.match(l.strip()) and not l.startswith(RUN_ID_PREFIX)]
    if pattern:
        rx = re.compile(pattern)
        lines = [l for l in lines if rx.search(l)]
    return lines[-tail:] if tail > 0 else []


def pane_argv(log: Path, command: list[str], run_id: str) -> list[str]:
    """gate_in_pane.sh running `command`, after stamping the log with this run's id."""
    stamped = ["sh", "-c", f'echo "{RUN_ID_PREFIX}$0"; exec "$@"', run_id, *command]
    return [str(GATE_IN_PANE), str(log), "--", *stamped]


def pane_command(log: Path, command: list[str], run_id: str) -> str:
    """pane_argv() as the one shell line `herdr pane run` takes."""
    return shlex.join(pane_argv(log, command, run_id))


def which_herdr(env: dict[str, str]) -> str | None:
    """"tui" inside the Herdr TUI, "app" inside the Herdr app, else None."""
    if env.get("HERDR_ENV") == "1":
        return "tui"
    if env.get("HC_TERMINAL_ID"):
        return "app"
    return None


def run_json(program: str, *args: str) -> dict:
    proc = subprocess.run([program, *args], capture_output=True, text=True)
    if proc.returncode != 0:
        raise RuntimeError(f"{program} {' '.join(args)} exited {proc.returncode}: "
                           f"{(proc.stdout + proc.stderr).strip()}")
    return json.loads(proc.stdout) if proc.stdout.strip() else {}


def start_tui_pane(argv: list[str], cwd: Path, direction: str) -> str:
    # Herdr sets HERDR_PANE_ID in every pane it starts: next to the pane
    # this runs in, not whichever pane has focus right now.
    own = os.environ.get("HERDR_PANE_ID")
    target = ["--pane", own] if own else ["--current"]
    split = herdr("pane", "split", *target, "--direction", direction,
                  "--cwd", str(cwd), "--no-focus")
    pane = split["result"]["pane"]["pane_id"]
    herdr("pane", "run", pane, shlex.join(argv))
    return pane


def start_app_terminal(argv: list[str], cwd: Path, direction: str, run_id: str) -> str:
    started = run_json("hc", "run", "--bg", "--json", "--no-notify",
                       "--name", f"pane-run-{run_id[:8]}", "--cwd", str(cwd), "--", *argv)
    address = started["address"]
    # The window learns of a new terminal a moment after hc starts it, and
    # until then answers "no running terminal" (measured 2026-11-03).
    for attempt in range(APP_RETRIES):
        try:
            run_json("herdr-app", "do", "terminal.keep", "--terminal", address,
                     "--place", direction)
            break
        except (RuntimeError, ValueError) as err:
            if attempt == APP_RETRIES - 1:
                # The run goes on without a pane (the window may be closed);
                # it still shows under the caller in the agents list.
                note(f"terminal {address} runs without a pane: {err}")
            else:
                time.sleep(APP_RETRY_S)
    return address


def remove_app_terminal(address: str) -> None:
    """`hc rm` once the program has exited: GATE_EXIT lands just before it does."""
    for _ in range(APP_RETRIES):
        if run_json("hc", "get", address, "--json").get("state") == "finished":
            break
        time.sleep(APP_RETRY_S)
    run_json("hc", "rm", address)


def claim_log(log: Path) -> int | None:
    """An fd holding <log>.pane-run.lock exclusively, or None when another run holds it.

    Held until this process exits; the kernel drops it if the process dies.
    """
    fd = os.open(f"{log}.pane-run.lock", os.O_RDWR | os.O_CREAT, 0o666)
    try:
        fcntl.flock(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except BlockingIOError:
        os.close(fd)
        return None
    return fd


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        description="Run a command in a new Herdr pane, wait for GATE_EXIT, close the pane.",
        usage="pane_run.py LOG [options] (--script FILE | -- command ...)",
    )
    p.add_argument("log", type=Path, help="log file; its last line becomes GATE_EXIT=<n>")
    p.add_argument("--script", type=Path, help="run `bash FILE` instead of a -- command")
    p.add_argument("--cwd", type=Path, default=Path.cwd(), help="pane working directory (default: here)")
    p.add_argument("--direction", choices=("right", "down"), default="right")
    p.add_argument("--tail", type=int, default=40, help="lines of the log to print (default 40)")
    p.add_argument("--grep", help="print only log lines matching this regex (then --tail)")
    p.add_argument("--timeout", type=float, default=7200, help="seconds to wait (default 7200)")
    p.add_argument("--interval", type=float, default=2.0, help="log poll interval in seconds")
    p.add_argument("--keep", action="store_true", help="leave the pane (TUI) or the finished hc terminal (app) afterwards")
    return p


def main(argv: list[str] | None = None) -> int:
    argv = list(sys.argv[1:] if argv is None else argv)
    command: list[str] = []
    if "--" in argv:
        split = argv.index("--")
        argv, command = argv[:split], argv[split + 1:]
    args = build_parser().parse_args(argv)
    if bool(command) == bool(args.script):
        note("give exactly one of --script FILE or -- command ...")
        return 2
    if args.script:
        script = args.script.resolve()
        if not script.is_file():
            note(f"no script at {script}")
            return 2
        command = ["bash", str(script)]
    kind = which_herdr(dict(os.environ))
    if kind is None:
        note("not inside Herdr (no HERDR_ENV=1 from the Herdr TUI, no HC_TERMINAL_ID from "
             "the Herdr app); run the command in a terminal yourself")
        return EXIT_NO_HERDR

    log = args.log.resolve()
    if claim_log(log) is None:
        note(f"another pane_run is waiting on {log}; name a log of your own")
        return EXIT_USAGE
    if log.exists():
        if gate_exit(log) is None:
            note(f"{log} has no GATE_EXIT yet: another run may still be writing it.")
            note("Name a log of your own, or delete this one if that run is gone.")
            return EXIT_USAGE
        note(f"replacing the finished log {log}")
        log.unlink()
    run_id = uuid.uuid4().hex

    argv_in_pane = pane_argv(log, command, run_id)
    cwd = args.cwd.resolve()
    try:
        if kind == "tui":
            pane = start_tui_pane(argv_in_pane, cwd, args.direction)
        else:
            pane = start_app_terminal(argv_in_pane, cwd, args.direction, run_id)
    except (RuntimeError, KeyError, ValueError) as err:
        note(f"could not start the pane: {err}")
        return EXIT_NO_HERDR
    note(f"{'pane' if kind == 'tui' else 'terminal'} {pane}: {shlex.join(command)}")
    note(f"log {log}")

    deadline = time.monotonic() + args.timeout
    code = gate_exit(log, run_id)
    while code is None:
        if time.monotonic() >= deadline:
            note(f"no GATE_EXIT after {args.timeout:.0f}s; pane {pane} left running")
            return EXIT_TIMEOUT
        time.sleep(args.interval)
        code = gate_exit(log, run_id)

    for line in summary(log, args.tail, args.grep):
        print(line)
    print(f"GATE_EXIT={code}")
    print(f"log: {log}")
    if not args.keep:
        try:
            if kind == "tui":
                herdr("pane", "close", pane)
            else:
                remove_app_terminal(pane)
        except (RuntimeError, ValueError) as err:
            note(f"could not close {pane}: {err}")
    return code


if __name__ == "__main__":
    sys.exit(main())
