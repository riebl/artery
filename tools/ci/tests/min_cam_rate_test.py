import math

from unittest import TestCase

from tools.ci.common import TestOptions, Decorators
from tools.ci.sim_results import SimRecordedData


# TODO: this test is naive, we should probably check time difference between
# vector records
@Decorators.artery_test
def min_cam_rate_test(test: TestCase, data: SimRecordedData, config: TestOptions):
    sentDown = data.scalars[data.scalars['scalarName'] == 'sentDownPk:count']
    simtime = data.vectors['endSimtimeRaw'].max() - data.vectors['startSimtimeRaw'].min()
    simtime_seconds = math.pow(simtime, data.simtimeExp)

    test.assertGreaterEqual(sentDown / simtime_seconds, config.get('min_cam_rate', 1))
