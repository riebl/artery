import inspect
import importlib

from pathlib import Path
from unittest import TestCase
from typing import Type, cast, Callable, Optional

from tools.run_artery import run_artery
from tools.ci.sim_results import SimRecordedData, SimResultsReader
from tools.ci.common import TestOptions, Decorators


class ArteryTestCaseBase(TestCase):
    """
    This class is responsible for collecting tests from tests/ directory
    and providing arguments to them, primarily configuration options and sim results.
    """

    # path to test implementations
    tests_path = Path.cwd() / 'tools/ci/tests'

    # default options for opp_run, used to retrieve required data
    run_options = {
        # switches for data recording
        '**.scalar-recording': 'true',
        '**.vector-recording': 'true',
        '**.statistic-recording': 'true',
        # output format - sqllite database
        'outputvectormanager-class': r'"omnetpp::envir::SqliteOutputVectorManager"',
        'outputscalarmanager-class': r'"omnetpp::envir::SqliteOutputScalarManager"',
        # express mode switch
        'cmdenv-express-mode': 'true',
        # resulting output files' name templates
        'output-vector-file': r'"${resultdir}/${configname}.vec"',
        'output-scalar-file': r'"${resultdir}/${configname}.sca"'
    }

    # these are set upon creating new type
    launch_conf: Path
    scenario_path: Path
    sim_results: SimRecordedData
    config: str
    test_options: TestOptions

    @classmethod
    def setUpClass(cls):
        opp_args = []
        for option, value in cls.run_options.items():
            opp_args.append(f'--{option}={value}')

        opp_args.extend(('-u', 'Cmdenv', '-c', cls.config))
        retval = run_artery(cls.launch_conf, opp_args, cls.scenario_path, mute_standard_fds=True)
        if retval != 0:
            raise RuntimeError(f'call to run_artery returned non-zero exit code: {retval}')

        reader = SimResultsReader()
        cls.sim_results = reader.read(cls.scenario_path, cls.config)

    @classmethod
    def load_tests(cls):
        if not cls.tests_path.is_dir():
            raise FileNotFoundError(f'directory {cls.tests_path} does not exist or is not a directory')
        
        for entity in cls.tests_path.iterdir():
            if not entity.is_file() or entity.suffix != '.py':
                continue

            cleared_path = entity.relative_to(Path.cwd()).with_suffix('')
            module = importlib.import_module(str(cleared_path).replace('/', '.'), '.')

            for name, func in inspect.getmembers(module, predicate=inspect.isfunction):
                if hasattr(func, Decorators.ARTERY_TEST_TAG):
                    for option, value in getattr(func, Decorators.ARTERY_OMNETPP_SETTINGS, {}).items():
                        cls.run_options[option] = value

                    wrapper = cls.__make_test(func)
                    setattr(cls, f'test_{cls.__name__}_{name}', wrapper)

    @classmethod
    def __make_test(cls, impl: Callable):
        def test(self):
            nonlocal cls
            impl(self, data=cls.sim_results, test_options=cls.test_options)

        return test
    

class ArteryTestFactory:
    @classmethod
    def make(
        cls,
        launch_conf: Path,
        scenario_path: Path,
        config: str = 'General',
        test_options: Optional[TestOptions] = None
    ) -> Type[ArteryTestCaseBase]:
        """
        Creates test for scenario from template, loading all requested tests
        and setting run configurations for Omnet++.

        Args:
            launch_conf (Path): path to launch configuration.
            scenario_path (Path): path to scenario under testing.
            config (Optional[str]): Specifies Omnet++ configuration to test, defaults to General.
            test_options (Optional[TestOptions]): provides options for test, defaults to empty dictionary.

        Returns:
            type: a test case type that runs Omnet++ on creation and runs tests against captured data.
        """
        scenario_name = scenario_path.stem.replace('-', '_')
        name = f'{scenario_name}_{config}_TestCase'

        if test_options is None:
            test_options = {}

        attrs = {
            attr: value
            for attr, value in zip(
                ('launch_conf', 'scenario_path', 'config', 'test_options'),
                (launch_conf, scenario_path, config, test_options)
            )
        }

        new_cls = type(name, (ArteryTestCaseBase, ), attrs)

        if not hasattr(new_cls, 'load_tests'):
            raise AttributeError('expected ArteryTestCaseBase to define load_tests class method')
        getattr(new_cls, 'load_tests')()

        return cast(Type[ArteryTestCaseBase], new_cls)
