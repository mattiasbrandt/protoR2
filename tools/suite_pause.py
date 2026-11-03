#!/usr/bin/env python3
"""Per-slice product suites do not run locally (#464).

A second agent's read of the production diff accepts a slice. The operator
ran that as an experiment from 2026-10-02 through 2026-10-31 and decided on
2026-11-03 to keep it: in that window the suites found no product defect,
and the reviews found the defects (#464 has the evidence). The tests stay in
the tree. CI runs the native suite and the web suite on a pull request into
main when its files reach them (.github/workflows/verification.yml
classifies the pull request's whole range; an epic's closure pull request
reaches both).

This skip covers local `make test`, `make test-web`, the native, web, and
mutation stages of slice_verify, and a direct `tools/mutation_verify.py`
run. It has no end date. PROTOR2_SUITES=1 runs them.

`--check` exits 0 when the suites are skipped and 1 when they should run,
so the Makefile can fall through.
"""

from __future__ import annotations

import os
import sys

BANNER = (
    "Per-slice product suites do not run locally (#464). "
    "Native tests, web tests, and mutation checks run in CI "
    "on a pull request into main when its files reach them. "
    "PROTOR2_SUITES=1 runs them here."
)


def paused() -> bool:
    return os.environ.get("PROTOR2_SUITES") != "1"


def main(argv: list[str]) -> int:
    active = paused()
    if "--check" in argv:
        if active:
            print(BANNER)
        return 0 if active else 1
    print(BANNER if active else "suites run (PROTOR2_SUITES=1)")
    return 0 if active else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
