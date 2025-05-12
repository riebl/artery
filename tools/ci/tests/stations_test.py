from unittest import TestCase

from tools.ci.artery_test_decorator import Config
from tools.ci.artery_test_decorator import artery_test
from tools.ci.sim_results import SimRecordedData


@artery_test
def test_vehicle_ids(test: TestCase, data: SimRecordedData, config: Config):
    test.assertTrue(data is not None)
    test.assertTrue(config is not None)
