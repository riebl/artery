import sys
import pytest

from pathlib import Path

# for type hints to work properly, enter stubs root
# to python path (import resolution path)
# for devcontainer: /workspaces/artery/src/artery/bindings/python


@pytest.fixture
def unload():
    def impl(stubs: str, bindings: str):
        for m in (stubs, bindings):
            sys.modules.pop(m, None)

    yield impl


sys.path.append(str(Path(__file__).parents[1] / 'python'))