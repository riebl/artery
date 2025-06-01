import enum
import sqlite3

from pathlib import Path
from functools import cached_property
from typing import Dict, Optional

import pandas as pd


class SimRecordedData:
    """Python interface to Recorded scalar & vector data"""

    class File(enum.StrEnum):
        SCALAR = 'scalar'
        VECTOR = 'vector'

    # Omnet++ saves data to .sca and .vec files, and data in those
    # differ. For example, statistics are written to .sca files, and .vec
    # files just contain empty table. This Loader must know which file
    # contains requested table, so this mapping is used.
    __file_mapping = {
        'run': File.SCALAR,
        'runAttr': File.SCALAR,
        'runItervar': File.SCALAR,
        'runParam': File.SCALAR,
        'scalar': File.SCALAR,
        'scalarAttr': File.SCALAR,
        'vector': File.VECTOR,
        'vectorAttr': File.VECTOR,
        'vectorData': File.VECTOR
    }

    __sql_quieries = {
        'run': 'SELECT * FROM run',
        'runAttr': 'SELECT * FROM runAttr',
        'runItervar': 'SELECT * FROM runItervar',
        'runParam': 'SELECT * FROM runParam',
        'scalar': 'SELECT * FROM scalar',
        'scalarAttr': 'SELECT * FROM scalarAttr',
        'vector': 'SELECT * FROM vector',
        'vectorAttr': 'SELECT * FROM vectorAttr',
        'vectorData': 'SELECT * FROM vectorData'
    }

    def __init__(self, vector_file: Path, scalar_file: Path):
        self.__vector_file = vector_file
        self.__scalar_file = scalar_file

    def __lazy_load(self, attr: str, private_field: Optional[str] = None) -> pd.DataFrame:
        if private_field is None:
            private_field = f'__{attr}'

        if getattr(self, private_field, None) is None:
            if attr not in self.__sql_quieries:
                raise KeyError(f'could not find query for attr: {attr}')
            if attr not in self.__file_mapping:
                raise KeyError(f'could not determine file type for attr: {attr}')
            
            match self.__file_mapping[attr]:
                case self.File.SCALAR:
                    file = self.__scalar_file
                case self.File.VECTOR:
                    file = self.__vector_file
                case _:
                    raise KeyError

            with sqlite3.connect(file) as ctx:
                df = pd.read_sql(self.__sql_quieries[attr], ctx)
                setattr(self, private_field, df)

        return getattr(self, private_field)
    
    @property
    def run(self) -> pd.DataFrame:
        return self.__lazy_load('run')

    @property
    def runAttr(self) -> pd.DataFrame:
        return self.__lazy_load('runAttr')

    @property
    def runItervar(self) -> pd.DataFrame:
        return self.__lazy_load('runItervar')

    @property
    def runParam(self) -> pd.DataFrame:
        return self.__lazy_load('runParam')

    @property
    def scalar(self) -> pd.DataFrame:
        return self.__lazy_load('scalar')

    @property
    def scalarAttr(self) -> pd.DataFrame:
        return self.__lazy_load('scalarAttr')

    @property
    def vector(self) -> pd.DataFrame:
        return self.__lazy_load('vector')

    @property
    def vectorAttr(self) -> pd.DataFrame:
        return self.__lazy_load('vectorAttr')

    @property
    def vectorData(self) -> pd.DataFrame:
        return self.__lazy_load('vectorData')
    
    @cached_property
    def simtimeExp(self) -> int:
        if self.run['simtimeExp'].shape[0] != 1:
            raise RuntimeError('simtimeExp property only works for single runs')

        return self.run['simtimeExp'][0]


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

        mapping = records[config]
        return SimRecordedData(mapping['vectors'], mapping['scalars'])
