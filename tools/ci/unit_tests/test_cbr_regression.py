import sqlite3
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from tools.ci.artery_test_case import ArteryTestCaseBase, ArteryTestFactory
from tools.ci.test_options_loader import ArteryTestConfigLoader


class CbrRegressionTest(unittest.TestCase):
    def run_cbr_case(self, rows, comparator, expected_success):
        with tempfile.TemporaryDirectory() as temporary_directory:
            root = Path(temporary_directory)
            scenario = root / 'scenario'
            results = scenario / 'results'
            config_directory = root / 'config'
            results.mkdir(parents=True)
            config_directory.mkdir()
            (results / 'General.vec').touch()

            with sqlite3.connect(results / 'General.sca') as connection:
                connection.execute(
                    'CREATE TABLE scalar (scalarName TEXT, scalarValue REAL)'
                )
                connection.executemany(
                    'INSERT INTO scalar VALUES (?, ?)',
                    [('ChannelLoad:timeavg' if is_cbr else 'Other:timeavg', value)
                     for is_cbr, value in rows]
                )

            (config_directory / 'case.yml').write_text(
                'test_runner: !test_runner\n'
                '  test_functions: [average_cbr_test]\n'
                '  tested_configurations: [General]\n'
                '  results_dir: results\n'
                'scenario: !scenario\n'
                f'  cbr: {comparator}\n'
                '  cbr_threshold: 1.0\n',
                encoding='utf-8'
            )
            with patch('tools.ci.artery_test_case.run_artery', return_value=0), \
                    patch.object(ArteryTestCaseBase, 'tests_path',
                                 Path(__file__).resolve().parents[1] / 'tests'):
                test_config = ArteryTestConfigLoader(config_directory).test_config('case')
                test_case = ArteryTestFactory.make(None, scenario, test_config)
                suite = unittest.defaultTestLoader.loadTestsFromTestCase(test_case)
                outcome = unittest.TestResult()
                suite.run(outcome)

            self.assertEqual(outcome.wasSuccessful(), expected_success,
                             outcome.errors + outcome.failures)
            self.assertEqual(len(outcome.errors), 0)
            return outcome

    def test_empty_and_unrelated_scalars_fail_both_comparators(self):
        for scalar_rows in ([], [(False, 2.0)]):
            for comparator in ('greater', 'less'):
                with self.subTest(rows=scalar_rows, comparator=comparator):
                    self.run_cbr_case(scalar_rows, comparator, False)

    def test_samples_preserve_threshold_and_nan_comparisons(self):
        cases = (
            ('greater', [(True, 1.1)], True),
            ('greater', [(True, 1.0)], False),
            ('greater', [(True, 1.1), (True, float('nan'))], False),
            ('less', [(True, 0.9)], True),
            ('less', [(True, 1.0)], False),
            ('less', [(True, 0.9), (True, float('nan'))], False),
        )
        for comparator, scalar_rows, expected_success in cases:
            with self.subTest(comparator=comparator, rows=scalar_rows):
                self.run_cbr_case(scalar_rows, comparator, expected_success)
