import pytest

from importlib import reload
from warnings import catch_warnings


@pytest.fixture(scope='module', autouse=True)
def binded_vanetza(unload):
    unload('vanetza', '_vanetza')
    with catch_warnings(record=True) as w:
        import vanetza.net
        reload(vanetza.net)

        assert not len(w), 'expected vanetza stub imports to pass without any error'
