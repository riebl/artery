import sys

from pathlib import Path

# for type hints to work properly, enter stubs root
# to python path (import resolution path)
# for devcontainer: /workspaces/artery/src/artery/bindings/python

sys.path.append(str(Path(__file__).parents[1] / 'python'))
