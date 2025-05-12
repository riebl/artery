from unittest import TestCase

from tools.ci.artery_test import Config
from tools.ci.artery_test import artery_test
from tools.ci.sim_results import SimRecordedData


@artery_test
def test_vehicle_ids(controller: TestCase, data: SimRecordedData, config: Config):
    controller.assertTrue(data is not None)
    controller.assertTrue(config is not None)
