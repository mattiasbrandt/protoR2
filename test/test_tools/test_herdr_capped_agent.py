"""Pinned behavior for tools/herdr_capped_agent.py that needs no live Herdr.

Starting an agent is not driven here (it needs a Herdr session and spends an
agent run). What is pinned: the cap the agent is started under, and that it
refuses rather than start an agent where it cannot cap or place it.
"""

import os
import sys
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools"))

import herdr_capped_agent  # noqa: E402


def _env_without_herdr():
    return {k: v for k, v in os.environ.items() if k not in ("HERDR_ENV", "HC_TERMINAL_ID")}


class Cap(unittest.TestCase):
    def test_the_app_agent_is_born_inside_the_capped_scope(self):
        argv = herdr_capped_agent.capped_argv("pa-agent-w-1.scope", "10G", ["claude", "--resume", "x"])
        self.assertEqual(argv[:5], ["systemd-run", "--user", "--scope", "--quiet",
                                    "--unit=pa-agent-w-1.scope"])
        for prop in ("MemoryMax=10G", "MemorySwapMax=0", "OOMPolicy=continue"):
            self.assertIn(prop, argv)
        self.assertEqual(argv[-3:], ["claude", "--resume", "x"])

    def test_sizes_compare_in_bytes_as_systemd_reports_them(self):
        self.assertEqual(herdr_capped_agent.to_bytes("10G"), 10737418240)
        self.assertEqual(herdr_capped_agent.to_bytes("512M"), 536870912)


class Refusals(unittest.TestCase):
    def test_outside_either_herdr_it_refuses(self):
        with mock.patch.dict("os.environ", _env_without_herdr(), clear=True):
            with self.assertRaises(SystemExit) as raised:
                herdr_capped_agent.main(["--name", "w"])
        self.assertIn("not inside Herdr", str(raised.exception))

    def test_the_tui_needs_a_pane(self):
        env = dict(_env_without_herdr(), HERDR_ENV="1")
        with mock.patch.dict("os.environ", env, clear=True):
            with self.assertRaises(SystemExit) as raised:
                herdr_capped_agent.main(["--name", "w"])
        self.assertIn("--pane is required", str(raised.exception))


if __name__ == "__main__":
    unittest.main()
