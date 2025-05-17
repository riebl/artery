from typing import Callable, TypeAlias, Dict, Union, Collection


TestOptions: TypeAlias = Dict[
    str, Union[str, 'TestOptions', Collection['TestOptions']]
]


class Decorators:
    ARTERY_TEST_TAG = 'artery_test_tag'
    ARTERY_OMNETPP_SETTINGS = 'artery_omnetpp_settings'

    @classmethod
    def with_omnetpp_settings(cls, settings: Dict[str, str]) -> Callable:
        """
        Appends (or replaces if specified already) default run options for Omnet++. Use
        This decorator only to request additional statistic/scalar/vector recordigs, since
        the whole test case will be run with these options.

        Args:
            settings (Dict[str, str]): Additional options.
        """
        def factory(test_func_impl: Callable):
            nonlocal settings
            setattr(test_func_impl, cls.ARTERY_OMNETPP_SETTINGS, settings)
            return test_func_impl

        return factory

    @classmethod
    def artery_test(cls, test_func_impl: Callable) -> Callable:
        """
        Marks method as Artery test case, required for ArteryTestCase class to determine
        which methods to load from tests/.
        """
        setattr(test_func_impl, cls.ARTERY_TEST_TAG, True)
        return test_func_impl

    # TODO: config defaults decorator. Right now tests define their config requirements
    # randomly - it should be moved into a single decorator 
