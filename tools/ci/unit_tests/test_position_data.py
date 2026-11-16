import sqlite3
import tempfile
import unittest
from pathlib import Path

from tools.ci.sim_results import SimRecordedData
from tools.ci.tests.coordinates_span_test import coordinates_span_test


class PositionDataRegressionTest(unittest.TestCase):
    def check_positions(self, vectors, samples=(), options=None):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / 'positions.vec'
            with sqlite3.connect(path) as database:
                database.execute(
                    'CREATE TABLE vector (vectorId INTEGER, moduleName TEXT, vectorName TEXT)'
                )
                database.execute('CREATE TABLE vectorData (vectorId INTEGER, value REAL)')
                database.executemany('INSERT INTO vector VALUES (?, ?, ?)', vectors)
                database.executemany('INSERT INTO vectorData VALUES (?, ?)', samples)
            data = SimRecordedData(vector_file=path, scalar_file=path)
            coordinates_span_test(test=self, data=data, test_options=options or {})

    def pair(self):
        return [(1, 'node[0]', 'posX:vector'), (2, 'node[0]', 'posY:vector')]

    def samples(self):
        return [(1, 0.0), (1, 5.0), (2, 1.0), (2, 3.0)]

    def assert_missing_positions(self, vectors):
        with self.assertRaisesRegex(AssertionError, 'paired posX/posY'):
            self.check_positions(vectors)

    def test_valid_pair_passes(self):
        self.check_positions(self.pair(), self.samples(), {'posX_min_span': 1})

    def test_only_x_fails(self):
        self.assert_missing_positions(self.pair()[:1])

    def test_only_y_fails(self):
        self.assert_missing_positions(self.pair()[1:])

    def test_different_modules_fail(self):
        self.assert_missing_positions([
            (1, 'node[0]', 'posX:vector'), (2, 'node[1]', 'posY:vector')
        ])

    def test_unrelated_vector_is_not_position_evidence(self):
        self.assert_missing_positions([(3, 'radio', 'transmission:vector')])

    def test_empty_metadata_fails(self):
        self.assert_missing_positions([])

    def test_valid_pair_with_unrelated_module_passes(self):
        self.check_positions(
            self.pair() + [(3, 'radio', 'transmission:vector')], self.samples()
        )

    def test_out_of_range_coordinates_still_fail(self):
        with self.assertRaises(AssertionError):
            self.check_positions(self.pair(), self.samples(), {'posX_max': 4})

    def test_out_of_range_span_still_fails(self):
        with self.assertRaisesRegex(AssertionError, 'X_span'):
            self.check_positions(self.pair(), self.samples(), {'posX_min_span': 6})

    def test_missing_samples_still_fail(self):
        with self.assertRaises(AssertionError):
            self.check_positions(self.pair())

    def test_all_coordinate_bounds_are_preserved(self):
        options = {
            'posX_min': 0, 'posX_max': 5, 'posX_min_span': 5, 'posX_max_span': 6,
            'posY_min': 1, 'posY_max': 3, 'posY_min_span': 2, 'posY_max_span': 3,
        }
        self.check_positions(self.pair(), self.samples(), options)
        for key, value in (
            ('posX_min', 1), ('posX_max', 4), ('posX_min_span', 6), ('posX_max_span', 5),
            ('posY_min', 2), ('posY_max', 2), ('posY_min_span', 3), ('posY_max_span', 2),
        ):
            with self.subTest(option=key), self.assertRaises(AssertionError):
                self.check_positions(self.pair(), self.samples(), {key: value})


if __name__ == '__main__':
    unittest.main()
