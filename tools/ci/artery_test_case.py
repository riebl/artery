import inspect
import importlib

from pathlib import Path
from unittest import TestCase
from typing import Type, Collection, Callable, Optional

from tools.run_artery import run_artery
from tools.ci.sim_results import SimRecordedData, SimResultsReader
from tools.ci.common import TestOptions, Decorators


class ArteryTestCaseTemplate(TestCase):
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
    sim_results: Collection[SimRecordedData]
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


class ArteryTestCaseFactory:
    def __init__(self, launch_conf: Path):     
        self.launch_conf = launch_conf

    def make_test_case(
        self,
        scenario_path: Path,
        config: str = 'General',
        test_options: Optional[TestOptions] = None
    ) -> type:
        """
        Creates test for scenario from template, loading all requested tests
        and setting run configurations for Omnet++.

        Args:
            scenario_path (Path): Path to scenario under testing.
            config (Optional[str]): Specifies Omnet++ configuration to test, defaults to General.
            test_options (Optional[TestOptions]): Provides options for test, defaults to empty dictionary.

        Returns:
            type: A test case type that runs Omnet++ on creation and runs tests against captured data.
        """
        scenario_name = scenario_path.stem.replace('-', '_')
        if test_options is None:
            test_options = {}

        test_case: Type[ArteryTestCaseTemplate] = type(
            f'{scenario_name}_{config}_TestCase',
            ArteryTestCaseTemplate.__bases__,
            dict(ArteryTestCaseTemplate.__dict__)
        )

        test_case.launch_conf = self.launch_conf
        test_case.scenario_path = scenario_path
        test_case.config = config
        test_case.test_options = test_options
        test_case.load_tests()
        return test_case
