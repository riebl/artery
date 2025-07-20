import sys
import pytest

from importlib import reload
from warnings import catch_warnings


@pytest.fixture(scope='module', autouse=True)
def stubed_vanetza(unload):
    unload('vanetza', '_vanetza')
    sys.modules['_vanetza'] = None # type: ignore

    with catch_warnings(record=True) as w:
        import vanetza.net
        reload(vanetza.net)

        assert len(w) == 1, 'expected stubs to warn about missing bindings'
