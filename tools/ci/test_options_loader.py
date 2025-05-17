import yaml

from pathlib import Path
from typing import Dict, Optional, List, Tuple

from tools.ci.common import TestOptions


class ArteryTestConfigLoader:
    # mappings for config values
    ENTRY = 'scenario'
    CONFIGURATIONS = 'tested_configurations'

    # reserved keys in configuration: cannot be names for options
    RESERVED = (CONFIGURATIONS)

    def __init__(self, config_dir: Path):
        self.__config_dir = config_dir
        self.__scenarios: Dict[str, Optional[TestOptions]] = {}

        for file in self.__config_dir.iterdir():
            if not file.is_file() or file.suffix != '.yml':
                continue
            self.__scenarios[file.stem] = None

    def __getitem__(self, key: Tuple[str, str]) -> TestOptions:
        scenario, config = key
        options, test_options = self.__lazy_load(scenario)[self.ENTRY], {}

        for option, value in options.items():
            if option in self.RESERVED:
                continue
            if option in options.get(self.CONFIGURATIONS, []):
                continue

            test_options[option] = value

        if config in options:
            for overriden_option, value in options[config]:
                test_options[overriden_option] = value

        return test_options
    
    def scenarios(self) -> List[str]:
        return self.__scenarios.keys()
    
    def configs(self, scenario: str) -> List[str]:
        return self.__lazy_load(scenario)[self.ENTRY].get(self.CONFIGURATIONS, [])

    def __lazy_load(self, scenario: str) -> TestOptions:
        if scenario not in self.__scenarios:
            raise KeyError(f'could not lazy-load scenario options: {scenario} not found')

        if self.__scenarios[scenario] is not None:
            return self.__scenarios[scenario]

        path = self.__config_dir / f'{scenario}.yml'
        if not path.is_file():
            raise FileNotFoundError(f'scenario was not found: {path}')
        
        with open(path) as fd:
            return yaml.safe_load(fd)