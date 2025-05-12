import yaml

from pathlib import Path
from typing import Dict, Optional, List

from tools.ci.artery_test_decorator import Config


class ArteryTestConfigLoader:
    def __init__(self, config_dir: Path):
        self.__config_dir = config_dir
        self.__scenarios: Dict[str, Optional[Config]] = {}

        for file in self.__config_dir.iterdir():
            if not file.is_file() or file.suffix != '.yml':
                continue
            self.__scenarios[file.stem] = None

    def __getitem__(self, scenario: str) -> Config:
        if scenario not in self.__scenarios:
            raise KeyError(
                f'scenario {scenario} was not found, please choose from ' + ', '.join(self.__scenarios.keys())
            )

        if self.__scenarios[scenario] is None:
            self.__scenarios[scenario] = self.__load(scenario)

        return self.__scenarios[scenario]
    
    def scenarios(self) -> List[str]:
        return self.__scenarios.keys()

    def __load(self, scenario: str) -> Config:
        path = self.__config_dir / f'{scenario}.yml'
        if not path.is_file():
            raise FileNotFoundError(f'scenario was not found: {path}')
        
        with open(path) as fd:
            return yaml.safe_load(fd)