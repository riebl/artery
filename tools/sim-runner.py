#!/usr/bin/env python3

import sys
import time
import enum
import argparse
import subprocess
import configparser

from pathlib import Path
from typing import Dict, Optional, Any


class RunnerMode(enum.StrEnum):
    BRIDGE = 'bridge'
    SILENT = 'silent'
    RECORD = 'record'


class SimRunner:

    RUNNER_BASE = {
        'General': {
            # switches for data recording
            '**.scalar-recording': 'true',
            '**.vector-recording': 'true',
            '**.statistic-recording': 'true',

            # output format - sqllite database
            'outputvectormanager-class': r'"omnetpp::envir::SqliteOutputVectorManager"',
            'outputscalarmanager-class': r'"omnetpp::envir::SqliteOutputScalarManager"',

            # express mode switch
            'cmdenv-express-mode': 'true',

            # resulting output files' name templates
            'output-vector-file': r'"${resultdir}/${configname}.vec"',
            'output-scalar-file': r'"${resultdir}/${configname}.sca"'
        }
    }

    def __init__(self, runner: Path, keep_runner_config: bool = False, frontend: str = 'Cmdenv'):
        self._keep_runner_config = keep_runner_config
        self._frontend = frontend
        self._runner = runner

    def run(self, scenario_directory: Path, user_options: Optional[Dict[str, Dict[str, Any]]] = None, mode: RunnerMode = RunnerMode.SILENT):
        if not isinstance(scenario_directory, Path):
            raise TypeError
        
        if not scenario_directory.is_dir():
            raise FileNotFoundError(f'scenario directory {scenario_directory} was not found')
        
        scenario_config_path = scenario_directory.joinpath('omnetpp.ini')
        runner_config_path = scenario_directory.joinpath('omnetpp.test.ini')
        self._make_config(runner_config_path, user_options)

        try:
            cmd = [self._runner, scenario_config_path, runner_config_path.name, '-u', self._frontend, '-m']
            result = None
            match mode:
                case RunnerMode.BRIDGE:
                    print('running command: ' + ' '.join(map(str, cmd)))
                    result = subprocess.run(cmd, cwd=scenario_directory, encoding='UTF-8', stdout=sys.stdout)
                case RunnerMode.SILENT:
                    result = subprocess.run(cmd, cwd=scenario_directory, encoding='UTF-8')
                case RunnerMode.RECORD:
                    result = subprocess.run(cmd, cwd=scenario_directory, encoding='UTF-8', capture_output=True) 

            if code := result.returncode:
                raise RuntimeError(f'runner failed with retirn code: {code}')
            
            if mode == RunnerMode.RECORD:
                self.stdout, self.stderr = result.stdout, result.stderr

        finally:
            if not self._keep_runner_config:
                runner_config_path.unlink()


    def _make_config(self, output_path: Path, user_options: Optional[Dict[str, Dict[str, Any]]] = None):
        if not isinstance(output_path, Path):
            raise TypeError
        
        config = configparser.ConfigParser(default_section='General')
        config.optionxform = str
        config.read_dict(SimRunner.RUNNER_BASE)
        if user_options is not None:
            config.read_dict(user_options)

        with open(output_path, 'w') as out:
            config.write(out)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(action='store', dest='script')
    parser.add_argument('-s', '--scenario', action='store', dest='scenario', required=True)
    parser.add_argument('-f', '--frontend', action='store', dest='frontend', default='Qtenv')
    parser.add_argument('--keep-runner-config', action='store_true', dest='keep_runner_config', default=False)

    args = parser.parse_args()
    runner = SimRunner(Path(args.script), args.keep_runner_config, args.frontend)

    try:
        runner.run(Path(args.scenario), mode=RunnerMode.BRIDGE)
    except KeyboardInterrupt:
        # TODO: wait for spawned process to finish
        print('aborted by user! waiting for OmnetPP to finish...')
        