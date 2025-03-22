#!/usr/bin/env python3

import sys
import time
import enum
import logging
import argparse
import subprocess
import configparser

from pathlib import Path
from typing import Dict, Optional, Any, Optional

from run_artery import run_artery

logger = logging.getLogger(__file__)


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

    def __init__(
        self,
        runall: bool = False,
        batch: Optional[int] = None,
        jobs: Optional[int] = None,
        frontend: str = 'Cmdenv'
    ):
        self.__runall = runall
        self.__batch = batch
        self.__jobs = jobs
        self.__frontend = frontend

    def run(
        self,
        runner_config: Path,
        scenario_directory: Path,
        scenario_config: Path,
        user_options: Optional[Dict[str, Dict[str, Any]]] = None
    ):
        if not isinstance(scenario_directory, Path):
            raise TypeError
        
        if not scenario_directory.is_dir():
            raise FileNotFoundError(f'scenario directory {scenario_directory} was not found')

        scenario_config_path = self.__resolve_scenario_config_path(scenario_directory, scenario_config)
        testing_config_path = scenario_directory.joinpath('.omnetpp.test.ini')
        self.__make_config(testing_config_path, user_options)

        try:
            returncode = run_artery(

            )

        finally:
            if not self.__keep_runner_config:
                runner_config_path.unlink()

    def __resolve_scenario_config_path(self, scenario_directory: Path, scenario_config: Path) -> Path:
        if scenario_config.is_absolute():
            return scenario_config
        try:
            resolved_path = scenario_config.relative_to(scenario_directory)
        except ValueError:
            raise ValueError(
                'scenario config path should be either absolute or relative to scenario directory'
            )
        return resolved_path
        

    def __make_config(self, output_path: Path, user_options: Optional[Dict[str, Dict[str, Any]]] = None):
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
        