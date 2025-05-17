import unittest
import argparse

from pathlib import Path
from tools.ci.artery_test_case import ArteryTestCaseFactory
from tools.ci.test_options_loader import ArteryTestConfigLoader


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-l', dest='launch_conf', action='store', type=Path)
    parser.add_argument('-s', dest='scenario_base_dir', action='store', type=Path)
    parser.add_argument('--verbosity', action='store', type=int, default=2)
    args = parser.parse_args()

    suite = unittest.TestSuite()
    loader = unittest.TestLoader()

    config_loader = ArteryTestConfigLoader(Path(__file__).parent / 'config')
    factory = ArteryTestCaseFactory(args.launch_conf)
    for scenario_name in config_loader.scenarios():
        scenario = args.scenario_base_dir / scenario_name
        if not scenario.is_dir():
            raise FileNotFoundError
        
        for config in config_loader.configs(scenario_name):
            test_case = factory.make_test_case(scenario, config, config_loader[scenario_name, config])
            suite.addTests(loader.loadTestsFromTestCase(test_case)._tests)

    runner = unittest.TextTestRunner(verbosity=args.verbosity)
    result = runner.run(suite)
    result.printErrors()


if __name__ == '__main__':
    main()
