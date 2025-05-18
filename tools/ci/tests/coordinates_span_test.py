import math

from unittest import TestCase

import pandas as pd

from tools.ci.sim_results import SimRecordedData
from tools.ci.common import TestOptions, Decorators


@Decorators.artery_test
@Decorators.with_omnetpp_settings({
    '**.posX.result-recording-modes': 'vector',
    '**.posY.result-recording-modes': 'vector'
})
def coordinates_span_test(test: TestCase, data: SimRecordedData, test_options: TestOptions):
    maskX = data.vectors['vectorName'].str.startswith('posX')
    maskY = data.vectors['vectorName'].str.startswith('posY')

    posX = data.vectors[maskX].rename(columns={'vectorId': 'vectorId_posX'})
    posY = data.vectors[maskY].rename(columns={'vectorId': 'vectorId_posY'})

    merged = pd.merge(posX, posY, on='moduleName', how='inner')
    filtered = merged[['moduleName', 'vectorId_posX', 'vectorId_posY']]

    for module, posX_id, posY_id in filtered.itertuples(index=False, name=None):
        X_values = data.vectorData[data.vectorData['vectorId'] == posX_id]['value']
        Y_values = data.vectorData[data.vectorData['vectorId'] == posY_id]['value']

        X_max, X_min = X_values.max(), X_values.min()
        Y_max, Y_min = Y_values.max(), Y_values.min()

        if 'posX_min' in test_options:
            test.assertGreaterEqual(X_min, test_options['posX_min'])
        if 'posX_max' in test_options:
            test.assertLessEqual(X_max, test_options['posX_max'])
        if 'posY_min' in test_options:
            test.assertGreaterEqual(Y_min, test_options['posY_min'])
        if 'posY_max' in test_options:
            test.assertLessEqual(Y_max, test_options['posY_max'])

        X_span = X_max - X_min
        posX_min_span = test_options.get('posX_min_span', 0)
        posX_max_span = test_options.get('posX_max_span', math.inf)
        test.assertTrue(
            posX_min_span <= X_span < posX_max_span,
            f'expected X_span {X_span} to be in ({posX_min_span}, {posX_max_span})'
        )

        Y_span = Y_max - Y_min
        posY_min_span = test_options.get('posY_min_span', 0)
        posY_max_span = test_options.get('posY_max_span', math.inf)
        test.assertTrue(
            posY_min_span <= Y_span < posY_max_span,
            f'expected Y_span {Y_span} to be in ({posY_min_span}, {posY_max_span})'
        )
        