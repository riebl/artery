import yaml

from pathlib import Path
from functools import lru_cache
from typing import Dict, Optional, List, Tuple, cast, Any, Iterable
from dataclasses import dataclass

from yaml import YAMLObject, Loader, SafeLoader, MappingNode

from tools.ci.common import TestOptions


@dataclass
class ArteryRunnerConfig(YAMLObject):
    """Options for test runner, commonly regarding test metaclass configuration"""

    yaml_tag = '!test_runner'
    yaml_loader = YAMLObject.yaml_loader + [SafeLoader]
    test_functions: List[str]
    tested_configurations: List[str]


class ArteryScenarioConfig(YAMLObject):
    """Options for testing functions, these are normally passed as-is into them"""

    yaml_tag = '!scenario'
    yaml_loader = YAMLObject.yaml_loader + [SafeLoader]

    def __init__(self, options: TestOptions, configurations: Dict[str, TestOptions]):
        self.__options = options
        self.__configurations = configurations

    def __getitem__(self, getter: Tuple[str, str]) -> Any:
        configuration, key = getter
        if configuration not in self.__configurations:
            return self.__options[key]
        
        return self.__configurations[configuration].get(key, self.__options[key])
    
    def __repr__(self) -> str:
        return repr({
            '__options': self.__options,
            '__configurations': self.__configurations
        })
    
    @lru_cache
    def keys(self, configuration: str) -> Iterable[str]:
        overriden_keys = self.__configurations.get(configuration, {}).keys()
        return set(overriden_keys) | set(self.__options.keys())

    @classmethod
    def from_yaml(cls, loader: Loader, node: MappingNode) -> 'ArteryScenarioConfig':
        raw = cast(Dict[str, Any], loader.construct_mapping(node, deep=True))
        overriden = raw.pop('configurations', {})
        return cls(options=raw, configurations=overriden)


class ArteryTestConfigLoader:
    def __init__(self, config_dir: Path):
        self.__config_dir = config_dir
        self.__scenarios: Dict[str, Optional[TestOptions]] = {}

        for file in self.__config_dir.iterdir():
            if not file.is_file() or file.suffix != '.yml':
                continue
            self.__scenarios[file.stem] = None

    def __getitem__(self, key: Tuple[str, str]) -> TestOptions:
        """
        Access scenario configuration, resolving overriden options.

        Args:
            key (Tuple[str, str]): pair of (scenario, configuration).

        Returns:
            TestOptions: loaded options.
        """
        scenario, config = key
        options = self.__lazy_load(scenario)

        # for option, value in options.items():
        #     if option in self.RESERVED:
        #         continue
        #     if option in options.get(self.CONFIGURATIONS, []):
        #         continue

        #     test_options[option] = value

        # if config in options:
        #     for overriden_option, value in options[config]:
        #         test_options[overriden_option] = value

        return {}
    
    def scenarios(self) -> List[str]:
        return list(self.__scenarios.keys())
    
    def configs(self, scenario: str) -> List[str]:
        return ['envmod']

    def __lazy_load(self, scenario: str) -> TestOptions:
        if scenario not in self.__scenarios:
            raise KeyError(f'could not lazy-load scenario options: {scenario} not found')

        if self.__scenarios[scenario] is not None:
            return cast(TestOptions, self.__scenarios[scenario])

        path = self.__config_dir / f'{scenario}.yml'
        if not path.is_file():
            raise FileNotFoundError(f'scenario was not found: {path}')
        
        with open(path) as fd:
            return yaml.safe_load(fd)
