import re

from unittest import TestCase

from tools.ci.sim_results import SimRecordedData
from tools.ci.common import TestOptions, Decorators


@Decorators.artery_test
@Decorators.defines_test_options({
    'vehicles': None
})
def stations_test(test: TestCase, data: SimRecordedData, test_options: TestOptions):
    uniq = data.vectors['moduleName'].unique()

    node_ids = set()
    pattern = re.compile(r'World\.node\[(\d*)\].*', flags=re.DOTALL)
    for record in uniq:
        if (match := re.match(pattern, record)) is None:
            continue

        node_ids.add(match.group(1))

    count = test_options['vehicles']
    test.assertEqual(
        len(node_ids),
        count,
        f'found stations with ids: {node_ids} of size {len(node_ids)}, expected: {count}'
    )