from unittest import TestCase

import numpy as np
import pandas as pd

from tools.ci.common import TestOptions, Decorators
from tools.ci.sim_results import SimRecordedData


@Decorators.artery_test
@Decorators.with_omnetpp_settings({
    '**.transmission.result-recording-modes': 'all',
    '**.transmission.result-recording-modes': 'all'
})
@Decorators.defines_test_options({
    'min_cam_rate': 0.1,
    'max_cam_rate': 1
})
def min_cam_rate_test(test: TestCase, data: SimRecordedData, test_options: TestOptions):
    generated_deltas = data.vector[data.vector['vectorName'] == 'transmission:vector(camGenerationDeltaTime)']
    station_ids = data.vector[data.vector['vectorName'] == 'transmission:vector(camStationId)']

    generated_deltas = generated_deltas[['vectorId', 'moduleName']].rename(columns={'vectorId': 'deltaVectorId'})
    station_ids = station_ids[['vectorId', 'moduleName']].rename(columns={'vectorId': 'stationVectorId'})

    transmissions = generated_deltas.merge(station_ids, on='moduleName', how='left')

    min_cam_rate = test_options['min_cam_rate']
    max_cam_rate = test_options['max_cam_rate']

    reordered = transmissions[['moduleName', 'deltaVectorId', 'stationVectorId']]
    for module, delta_vector_id, station_vichle_id in reordered.itertuples(index=False):
        ids = data.vectorData[data.vectorData['vectorId'] == station_vichle_id][['value']].rename(columns={'value': 'id'})
        timestamps = data.vectorData[data.vectorData['vectorId'] == delta_vector_id][['value']].rename(columns={'value': 'stamp'})
        transmission = pd.merge(ids.reset_index(), timestamps.reset_index(), left_index=True, right_index=True)

        for id in transmission['id'].unique():
            stamps_for_id = transmission[transmission['id'] == id]['stamp']

            ndarray = np.arange(stamps_for_id.shape[0] - 1)
            for current, next in zip(ndarray, ndarray + 1):
                # time is recorded in ms
                delta = round((stamps_for_id.iat[next] - stamps_for_id.iat[current]) * 0.001, 2)
                test.assertTrue(
                    min_cam_rate <= delta <= max_cam_rate,
                    f'{module}: cam rate {delta} is out of range [{min_cam_rate}; {max_cam_rate}]'
                )
