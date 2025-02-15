import rich
import subprocess

import pandas as pd

from pathlib import Path
from typing import Union, Optional, Dict, List

from dataclasses import dataclass


@dataclass
class SimRecordedData:
    config_name: str
    scalars: Optional[pd.DataFrame] = None
    vectors: Optional[pd.DataFrame] = None

    def __bool__(self):
        return self.scalars is not None or self.vectors is not None


class SimResultsReader:

    def __init__(self, keep_intermidiate_data: bool = False, overwrite_intermidiate_data: bool = False):
        self._keep_intermidiate_data = keep_intermidiate_data
        self._overwrite_intermidiate_data = overwrite_intermidiate_data

    def _invoke_scavetool(self, results_file: Path, output: Optional[Union[Path, str]] = None) -> Path:
        if output is None or isinstance(output, str):
            if output is None:
                output = results_file.with_suffix('.csv')
            if isinstance(output, str):
                output = results_file.with_suffix(output)
            output = output.with_stem(f'{output.stem}-{results_file.suffix[1:]}')
        
        if not isinstance(output, Path):
            raise TypeError
        if not isinstance(results_file, Path):
            raise TypeError
        
        results_file, output = results_file.resolve(), output.resolve()
        if not results_file.is_file():
            raise FileNotFoundError(f'simulation results file could not be located: {results_file}')
        if output.is_file():
            if self._overwrite_intermidiate_data:
                output.unlink()
            else:
                raise FileExistsError(f'processed simulation results file already exists: {output}')
        
        process = subprocess.run(['scavetool', 'x', results_file, '-o', output], capture_output=True)
        if process.returncode:
            raise RuntimeError(f'running scavetool failed, command\'s stderr: {process.stderr}')

        return output
        
    def _scan_results(self, scenerio_path: Path) -> Dict[str, Dict[str, Path]]:
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

    def _handle_intermidiate_file(self, filepath: Path):
        if filepath.is_file():
            if self._keep_intermidiate_data:
                return
            filepath.unlink()
            return
        raise FileNotFoundError(f'could not find intermidiate file: {filepath}')
        
    
    def read(self, scenerio_path: Path) -> List[SimRecordedData]:
        records = self._scan_results(scenerio_path)

        data = []
        for config, path_mapping in records.items():
            recording = SimRecordedData(config)
            if 'vectors' in path_mapping:
                csv_filepath = self._invoke_scavetool(path_mapping['vectors'], '.csv')
                recording.vectors = pd.read_csv(csv_filepath)
                self._handle_intermidiate_file(csv_filepath)
            if 'scalars' in path_mapping:
                csv_filepath = self._invoke_scavetool(path_mapping['scalars'], '.csv')
                recording.scalars = pd.read_csv(csv_filepath)
                self._handle_intermidiate_file(csv_filepath)
    
            if recording:
                data.append(recording)

        return data
                


reader = SimResultsReader(overwrite_intermidiate_data=True)
for recording in reader.read(Path('/workspaces/artery/scenarios/highway-police')):
    print(recording.config_name)
    vec = recording.vectors
    sca = recording.scalars
    rich.print(vec.columns)
    # rich.print(*vec['module'].unique(), sep='\n')
    # 1
    # rich.print(vec[vec['module'] == 'World.node[3].wlan[0].radio'][vec['type'] == 'attr'])
    # rich.print(vec['type'].unique())
    # rich.print(vec[vec['type'] == 'vector'])
    # rich.print(*vec[vec['type'] == 'vector']['vecvalue'].unique(), sep='\n')
    # 2
    # rich.print(vec[vec['type'] == 'vector'][['vecvalue', 'module']].sample(30))
    rich.print(sca.columns)
    rich.print(sca['type'].unique())
    rich.print(sca[sca['type'] == 'attr']['name'].unique())
    # rich.print(sca[sca['max'].notna()][['max', 'min', 'name']], sep='\n')