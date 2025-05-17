import re

from unittest import TestCase

from tools.ci.sim_results import SimRecordedData
from tools.ci.common import TestOptions, Decorators


@Decorators.artery_test
def stations_test(test: TestCase, data: SimRecordedData, config: TestOptions):
    test.assertTrue('vehicles' in config, 'vehicle count was not provided by config')
    uniq = data.vectors['moduleName'].unique()

    node_ids = set()
    pattern = re.compile(r'World\.node\[(\d*)\].*', flags=re.DOTALL)
    for record in uniq:
        if (match := re.match(pattern, record)) is None:
            continue

        node_ids.add(match.group(1))

    count = config['vehicles']
    test.assertEqual(
        len(node_ids),
        count,
        f'found stations with ids: {node_ids} of size {len(node_ids)}, expected: {count}'
    )