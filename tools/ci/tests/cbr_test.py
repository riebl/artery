from unittest import TestCase

from tools.ci.sim_results import SimRecordedData
from tools.ci.common import TestOptions, ArteryTest


@ArteryTest.artery_test
@ArteryTest.with_omnetpp_settings({
    '**.ChannelLoad.scalar': 'true'
})
@ArteryTest.defines_test_options({
    'cbr': 'greater',
    'cbr_threshold': 0.0
})
def average_cbr_test(test: TestCase, data: SimRecordedData, test_options: TestOptions):
    cbr, threshold = test_options['cbr'], test_options['cbr_threshold']

    mask = data.scalar['scalarName'] == 'ChannelLoad:timeavg'
    cbr_values = data.scalar[mask]['scalarValue']
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
