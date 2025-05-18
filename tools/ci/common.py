from functools import wraps
from unittest import TestCase
from typing import Callable, TypeAlias, Dict, Union, Collection, Optional


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
        return test_func_impl\
        
    @classmethod
    def defines_test_options(cls, test_options: Dict[str, Optional[str]]) -> Callable:
        """
        Preprocesses "test_options" argument, inserting default values. If default value is None,
        then this wrapper raises error.

        Args:
            test_options (Dict[str, str]): options and their default values, provide None for option if it is required.
        """
        def factory(test_func_impl: Callable):
            @wraps(test_func_impl)
            def wrapper(*args, **kwargs):
                nonlocal test_options
                if 'test_options' not in kwargs:
                    raise ValueError('missing "test_options" in args of test function')
            
                for option, value in test_options.items():
                    if option not in kwargs['test_options']:
                        if value is None:
                            test: TestCase = kwargs['test']
                            test.fail(
                                f'required argument is not supplied: {option}, please edit yaml test options'
                            )
                        kwargs['test_options'][option] = value
        
                return test_func_impl(*args, **kwargs)
            
            return wrapper
        return factory
