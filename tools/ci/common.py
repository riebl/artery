from functools import wraps
from unittest import TestCase
from typing import Callable, TypeAlias, Dict, Union, Collection, Optional, Any


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
        def factory(test_func_impl: Callable) -> Callable:
            nonlocal settings
            setattr(test_func_impl, cls.ARTERY_OMNETPP_SETTINGS, settings)
            return test_func_impl

        return factory
    
    @classmethod
    def artery_test(cls, test_func_impl: Callable) -> Callable:
        """
        Marks method as Artery test case, required for ArteryTestCase class to determine
        which methods to load from tests.
        """
        setattr(test_func_impl, cls.ARTERY_TEST_TAG, test_func_impl.__name__)
        return test_func_impl

    @classmethod
    def artery_test_with_name(cls, name: Optional[str] = None) -> Callable:
        """
        Same as Decorators.artery_test, but supports specifying overriden function name,
        which is visible to yaml configs.

        Args:
            name (Optional[str]): overriden name for this test. Takes function identifier as default.
        """
        def factory(test_func_impl: Callable) -> Callable:
            nonlocal name
            if name is None:
                return cls.artery_test(test_func_impl)
            
            decorated = cls.artery_test(test_func_impl)
            setattr(decorated, cls.ARTERY_TEST_TAG, name)
            return decorated
        
        return factory
        
    @classmethod
    def defines_test_options(cls, test_options: Dict[str, Optional[Any]]) -> Callable:
        """
        Preprocesses "test_options" argument, inserting default values. If default value is None,
        then this wrapper raises error.

        Args:
            test_options (Dict[str, str]): options and their default values, provide None for option if it is required.
        """
        def factory(test_func_impl: Callable) -> Callable:
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
