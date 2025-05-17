import sqlite3
import pandas as pd

from pathlib import Path
from dataclasses import dataclass
from typing import Optional, Dict


@dataclass
class SimRecordedData:
    # TODO: expend to all all tables as needed
    # TODO: support multiple runs?
    config_name: str
    simtimeExp: Optional[int] = None
    scalars: Optional[pd.DataFrame] = None
    vectors: Optional[pd.DataFrame] = None
    vectorData: Optional[pd.DataFrame] = None

    def __bool__(self):
        return all(data is not None for data in (self.scalars, self.vectors))


class SimResultsReader:
    def __scan_results(self, scenerio_path: Path) -> Dict[str, Dict[str, Path]]:
        if not isinstance(scenerio_path, Path):
            raise TypeError
        
        scenerio_path = scenerio_path.resolve()
        if not scenerio_path.is_dir():
            raise FileNotFoundError(f'scenerio folder was not found: {scenerio_path}')
        # sanity check
        results_dir = scenerio_path.joinpath('results')
        if not results_dir.is_dir():
            raise RuntimeError(f'sanity check failed for directory: {scenerio_path}; results directory was not found')
        
        records = {}
        key_mappings = {'.sca': 'scalars', '.vec': 'vectors'}

        for child in results_dir.iterdir():
            if child.match('*.sca') or child.match('*.vec'):
                config_name = child.stem
                if config_name not in records:
                    records[config_name] = {}
                records[config_name][key_mappings[child.suffix]] = child 

        return records
    
    def read(self, scenerio_path: Path, config: str) -> SimRecordedData:
        records = self.__scan_results(scenerio_path)
        if config not in records:
            raise KeyError(f'run results for config {config} not found in ' + ', '.join(records.keys()))

        recording = SimRecordedData(config)
        path_mapping = records[config]
        if 'vectors' in path_mapping:
            with sqlite3.connect(path_mapping['vectors']) as ctx:
                recording.vectors = pd.read_sql('SELECT * FROM vector', ctx)
                recording.vectorData = pd.read_sql('SELECT * FROM vectorData', ctx)
        if 'scalars' in path_mapping:
            with sqlite3.connect(path_mapping['scalars']) as ctx:
                recording.scalars = pd.read_sql('SELECT * FROM scalar', ctx)
                cursor = ctx.execute('SELECT simtimeExp FROM run')
                # TODO: dataclasses should be capable of parsing this
                recording.simtimeExp = cursor.fetchall()[0][0]

        return recording
