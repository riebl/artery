from typing import Callable, TypeAlias, Dict, Union, Collection


Config: TypeAlias = Dict[str, Union[str, 'Config', Collection['Config']]]


def artery_test(test_func_impl: Callable):
    setattr(test_func_impl, 'is_artery_test', True)
    return test_func_impl
