from unittest import TestCase

from tools.ci.sim_results import SimRecordedData
from tools.ci.common import TestOptions, Decorators


# TODO: also support all_close cbr evaluation policy
# TODO: also support veins model 
@Decorators.artery_test
@Decorators.with_omnetpp_settings({
    '**.ChannelLoad.scalar': 'true'
})
def cbr_test(test: TestCase, data: SimRecordedData, config: TestOptions):
    cbr = config.get('cbr', 'greater')
    threshold = config.get('cbr_threshold', 0.0)

    mask = data.scalars['scalarName'] == 'ChannelLoad:timeavg'
    cbr_values = data.scalars[mask]['scalarValue']
    match cbr:
        case 'greater':
            test.assertTrue(
                all(value > threshold for value in cbr_values),
                f'some CBR values are less than {threshold}: {cbr_values}'
            )
        case 'less':
            test.assertTrue(
                all(value < threshold for value in cbr_values),
                f'some CBR values are greater than {threshold}: {cbr_values}'
            )
        case _:
            test.fail(f'could not resolve cbr config value: {cbr} - test configuration problem')
