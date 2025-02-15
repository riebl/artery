import yaml

from pathlib import Path
from unittest import TestLoader, TestSuite
from typing import override


class ArteryTestLoader(TestLoader):
    ...