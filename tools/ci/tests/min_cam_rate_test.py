from math import pow
from unittest import TestCase

import numpy as np
import pandas as pd

from tools.ci.common import TestOptions, ArteryTest
from tools.ci.sim_results import SimRecordedData


@ArteryTest.artery_test
@ArteryTest.with_omnetpp_settings({
    '**.transmission.result-recording-modes': 'all',
    '**.transmission.result-recording-modes': 'all'
})
@ArteryTest.defines_test_options({
    'min_cam_rate': 0.1,
    'max_cam_rate': 1
})
def min_cam_rate_test(test: TestCase, data: SimRecordedData, test_options: TestOptions):
    generated_deltas = data.vector[data.vector['vectorName'] == 'transmission:vector(camGenerationDeltaTime)']
    station_ids = data.vector[data.vector['vectorName'] == 'transmission:vector(camStationId)']

    generated_deltas = generated_deltas[['vectorId', 'moduleName']].rename(columns={'vectorId': 'deltaVectorId'})
    station_ids = station_ids[['vectorId', 'moduleName']].rename(columns={'vectorId': 'stationVectorId'})

    transmissions = generated_deltas.merge(station_ids, on='moduleName', how='left')

    exp = data.simtimeExp
    min_cam_rate = test_options['min_cam_rate']
    max_cam_rate = test_options['max_cam_rate']

    reordered = transmissions[['moduleName', 'deltaVectorId', 'stationVectorId']]
    for module, delta_vec_id, station_vec_id in reordered.itertuples(index=False):
        ids = data.vectorData[data.vectorData['vectorId'] == station_vec_id][['value']].rename(columns={'value': 'id'})
        timestamps = data.vectorData[data.vectorData['vectorId'] == delta_vec_id][['simtimeRaw']].rename(columns={'simtimeRaw': 'stamp'})
        transmission = pd.merge(ids.reset_index(), timestamps.reset_index(), left_index=True, right_index=True)

        for id in transmission['id'].unique():
            stamps_for_id = transmission[transmission['id'] == id]['stamp']

            ndarray = np.arange(stamps_for_id.shape[0] - 1)
            for current, next in zip(ndarray, ndarray + 1):
                # simtime -> seconds
                delta = round(pow(10, exp) * (stamps_for_id.iat[next] - stamps_for_id.iat[current]), 2)
                test.assertTrue(
                    min_cam_rate <= delta <= max_cam_rate,
                    f'{module}: cam rate {delta} is out of range [{min_cam_rate}; {max_cam_rate}]'
                )
