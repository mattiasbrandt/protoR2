#!/bin/sh
# Run a slice gate in a pane without losing its exit code.
#
# A pipe (`gate 2>&1 | tee log`) records tee's status. This runs the command
# with both streams in the log, restores the two version stamps a firmware
# build rewrites, then appends GATE_EXIT=<n> so that line stays last.
#
#   tools/gate_in_pane.sh /tmp/gate.log -- \
#     python3 tools/slice_verify.py --base <ref> --json /tmp/gate.json
#
# /tmp/gate.json has a boolean "ok". The log's last line is the exit code.
#
# In a pane (stdout is a terminal) the log is also followed on screen, so the
# operator sees the run; the log is still what is verified. Until 2026-11-03
# the pane stayed blank for the whole run, and a blank pane looks dead: one
# was closed 23 s into a build, which killed it. Python is unbuffered so its
# lines reach the log, and the screen, as they are printed.
set -u
if [ "$#" -lt 3 ] || [ "$2" != "--" ]; then
  echo "usage: tools/gate_in_pane.sh LOG -- command..." >&2
  exit 2
fi
log=$1
shift 2
: >"$log"
follow=
if [ -t 1 ]; then
  tail -n +1 -f "$log" &
  follow=$!
fi
PYTHONUNBUFFERED=1 "$@" >>"$log" 2>&1
code=$?
git checkout -- data/fw-version.json data/fs-version.json >>"$log" 2>&1 || true
printf 'GATE_EXIT=%s\n' "$code" >>"$log"
if [ -n "$follow" ]; then
  sleep 1  # let tail print the last lines before it stops
  kill "$follow" 2>/dev/null
fi
exit "$code"
