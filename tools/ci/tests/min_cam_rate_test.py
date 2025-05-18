import math

from unittest import TestCase

from tools.ci.common import TestOptions, Decorators
from tools.ci.sim_results import SimRecordedData


# TODO: this test is naive, we should probably check time difference between
# vector records
@Decorators.artery_test
@Decorators.defines_test_options({
    'min_cam_rate': 1
})
def min_cam_rate_test(test: TestCase, data: SimRecordedData, test_options: TestOptions):
    sentDown = data.scalars[data.scalars['scalarName'] == 'sentDownPk:count']['scalarValue']
    simtime = data.vectors['endSimtimeRaw'].max() - data.vectors['startSimtimeRaw'].min()
    simtime_seconds = math.pow(simtime, data.simtimeExp)

    for sentDownCount in sentDown:
        test.assertGreaterEqual(sentDownCount / simtime_seconds, test_options['min_cam_rate'])
