import uuid
import pytest

from functools import lru_cache


def pytest_addoption(parser):
    parser.addoption(
        '--reports-dir',
        action='store',
        default='./.pytest-reports',
        help='Root directory for test artifacts',
    )


@pytest.fixture
@lru_cache
def run_id() -> int:
    """ Get unique among runs number to perform run-specific actions. """
    return uuid.uuid4().int
