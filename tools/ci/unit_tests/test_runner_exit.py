import sqlite3
import sys
import tempfile
import unittest
from contextlib import redirect_stderr, redirect_stdout
from io import StringIO
from pathlib import Path
from unittest.mock import patch

from tools.ci import run
from tools.ci.artery_test_case import ArteryTestCaseBase
from tools.ci.test_options_loader import ArteryTestConfigLoader


class RunnerExitTest(unittest.TestCase):
    def run_cli_case(self, scalar_rows, scalar_table=True):
        with tempfile.TemporaryDirectory() as temporary_directory:
            root = Path(temporary_directory)
            scenario = root / 'case'
            results = scenario / 'results'
            config_directory = root / 'config'
            results.mkdir(parents=True)
            config_directory.mkdir()
            (results / 'General.vec').touch()
            with sqlite3.connect(results / 'General.sca') as connection:
                if scalar_table:
                    connection.execute(
                        'CREATE TABLE scalar (scalarName TEXT, scalarValue REAL)'
                    )
                    connection.executemany(
                        'INSERT INTO scalar VALUES (?, ?)', scalar_rows
                    )
            (config_directory / 'case.yml').write_text(
                'test_runner: !test_runner\n'
                '  test_functions: [average_cbr_test]\n'
                '  tested_configurations: [General]\n'
                '  results_dir: results\n'
                'scenario: !scenario\n'
                '  cbr: greater\n'
                '  cbr_threshold: 1.0\n',
                encoding='utf-8'
            )
            original_loader = ArteryTestConfigLoader

            def fixture_loader(_config_directory):
                return original_loader(config_directory)

            arguments = [
                'run.py', '-s', str(root), '-l', 'unused-launch-config',
                '--verbosity', '0'
            ]
            with patch.object(sys, 'argv', arguments), \
                    patch.object(run, 'ArteryTestConfigLoader', fixture_loader), \
                    patch('tools.ci.artery_test_case.run_artery', return_value=0), \
                    patch.object(ArteryTestCaseBase, 'tests_path',
                                 Path(__file__).resolve().parents[1] / 'tests'), \
                    redirect_stdout(StringIO()), redirect_stderr(StringIO()):
                try:
                    run.main()
                except SystemExit as exit_signal:
                    return exit_signal.code
                return 0

    def test_cli_returns_zero_for_passing_suite(self):
        self.assertEqual(self.run_cli_case([('ChannelLoad:timeavg', 1.1)]), 0)

    def test_cli_returns_one_for_empty_cbr_failure(self):
        self.assertEqual(self.run_cli_case([]), 1)

    def test_cli_returns_one_for_threshold_failure_with_observation(self):
        self.assertEqual(self.run_cli_case([('ChannelLoad:timeavg', 0.9)]), 1)

    def test_cli_returns_one_for_suite_error(self):
        self.assertEqual(self.run_cli_case([], scalar_table=False), 1)
