import unittest
import argparse

from pathlib import Path

import nose2

from rich.traceback import install as install_rich_traceback

from tools.ci.artery_test_case import ArteryTestFactory
from tools.ci.test_options_loader import ArteryTestConfigLoader


def main():
    install_rich_traceback(show_locals=True)
    parser = argparse.ArgumentParser()

    parser.add_argument('-l', dest='launch_conf', action='store', type=Path)
    parser.add_argument('-s', dest='scenario_base_dir', action='store', type=Path)
    parser.add_argument('--verbosity', action='store', type=int, default=2)
    args = parser.parse_args()

    suite = unittest.TestSuite()
    loader = unittest.TestLoader()

    config_loader = ArteryTestConfigLoader(Path(__file__).parent / 'config')
    for scenario_name in config_loader.scenarios():
        scenario_path = args.scenario_base_dir / scenario_name
        if not scenario_path.is_dir():
            raise FileNotFoundError
        
        test_config = config_loader.test_config(scenario_name)
        for config in test_config.test_runner.tested_configurations:
            test_case = ArteryTestFactory.make(args.launch_conf, scenario_path, test_config, config)
            suite.addTests(loader.loadTestsFromTestCase(test_case)._tests)

    runner = unittest.TextTestRunner(verbosity=args.verbosity)
    result = runner.run(suite)
    result.printErrors()


if __name__ == '__main__':
    main()
