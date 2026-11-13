import unittest
from pathlib import Path
from unittest.mock import patch

from tools.ci.artery_test_case import ArteryTestCaseBase, ArteryTestFactory
from tools.ci.test_options_loader import (
    ArteryRunnerConfig,
    ArteryScenarioConfig,
    ArteryTestConfig,
)


class TimeLimitRegressionTest(unittest.TestCase):
    def setUp(self):
        options = patch.object(
            ArteryTestCaseBase, 'run_options', dict(ArteryTestCaseBase.run_options)
        )
        options.start()
        self.addCleanup(options.stop)
        tests_path = patch.object(
            ArteryTestCaseBase, 'tests_path', Path(__file__).resolve().parents[1] / 'tests'
        )
        tests_path.start()
        self.addCleanup(tests_path.stop)

    def make_case(self, limit, test_functions=()):
        runner = ArteryRunnerConfig(
            test_functions=list(test_functions),
            tested_configurations=['General'],
            sim_time_limit=limit,
        )
        config = ArteryTestConfig(runner, ArteryScenarioConfig({}, {}))
        return ArteryTestFactory.make(Path('launch.ini'), Path('scenario'), config)

    def arguments(self, case):
        with patch('tools.ci.artery_test_case.run_artery', return_value=0) as run, \
                patch('tools.ci.artery_test_case.SimResultsReader'):
            case.setUpClass()
        self.assertEqual(run.call_args.args[0], Path('launch.ini'))
        self.assertEqual(run.call_args.args[2], Path('scenario'))
        self.assertTrue(run.call_args.kwargs['capture_output'])
        return run.call_args.args[1]

    def test_configured_limits_reach_simulator(self):
        for limit in (45, 90):
            with self.subTest(limit=limit):
                args = self.arguments(self.make_case(limit))
                self.assertEqual(
                    [arg for arg in args if arg.startswith('--sim-time-limit=')],
                    [f'--sim-time-limit={limit}s'],
                )

    def test_omitted_limit_leaves_scenario_default(self):
        args = self.arguments(self.make_case(None))
        self.assertFalse(any(arg.startswith('--sim-time-limit=') for arg in args))
        self.assertIn('--**.scalar-recording=true', args)
        self.assertEqual(args[-4:], ['-u', 'Cmdenv', '-c', 'General'])

    def test_explicit_limit_does_not_leak_to_next_scenario(self):
        self.arguments(self.make_case(90))
        args = self.arguments(self.make_case(None))
        self.assertFalse(any(arg.startswith('--sim-time-limit=') for arg in args))


if __name__ == '__main__':
    unittest.main()
