#!/usr/bin/env python3

import os
import sys
import errno
import shutil
import typing
import pathlib
import argparse
import subprocess
import multiprocessing

from dataclasses import dataclass, field

@dataclass
class Config:
    build_directory: pathlib.Path = field(default_factory=lambda: pathlib.Path.cwd().joinpath('build'))
    build_configs: typing.List[str] = field(default_factory=lambda: ['Debug'])
    cores: int = multiprocessing.cpu_count()
    profile: typing.Union[pathlib.Path, str] = 'default' 


class Routines:

    def __init__(self: 'Routines', params: Config) -> None:
        self._params = params

    def routines(self: 'Routines') -> typing.Generator[typing.Tuple[str, typing.Callable], None, None]:
        for routine in ['remove', 'install', 'configure', 'build', 'link']:
            yield routine, getattr(self, routine)

    def remove(self: 'Routines') -> None:
        for config in self._params.build_configs:
            directory = self._params.build_directory.joinpath(config)
            if directory.is_dir():
                print(f'removing directory for CMake build config \'{config}\'')
                shutil.rmtree(directory)
            else:
                print(f'build directory for config \'{config}\' was not found or was not a directory')

    def install(self: 'Routines') -> None:
        for config in self._params.build_configs:
            print(f'running conan install command for CMake config {config}')
            self._run([
                'conan', 
                'install',
                '--build=missing',
                f'-pr:a={self._params.profile}',
                f'--settings=build_type={config}',
                f'--conf=user.recipe:build_dir={self._params.build_directory}',
                str(pathlib.Path.cwd().joinpath('devcontainers'))
            ])

    def configure(self: 'Routines') -> None:
        if not self._params.build_directory.is_dir():
            self._params.build_directory.mkdir()

        use_presets = pathlib.Path.cwd().joinpath('CMakeUserPresets.json').is_file()
        print('configuring for CMake build configs: ' + ', '.join(self._params.build_configs))

        for config in self._params.build_configs:
            source = pathlib.Path.cwd()
            binary = self._params.build_directory.joinpath(config)
            command = [
                'cmake',
                '--preset', f'conan-{config.lower()}',
                '-B', str(binary), 
                '-S', str(source), 
                self._decorate_cmake_variable('CMAKE_BUILD_TYPE', config)
            ]
            if use_presets:
                command += []
            print(f'running configure command for CMake config {config}')
            self._run(command)

    def build(self: 'Routines') -> None:
        if not self._params.build_directory.is_dir():
            sys.exit(f'build directory \'{self._params.build_directory}\' was not found')

        print(f'using {self._params.cores} threads')
        for config in self._params.build_configs:
            directory = self._params.build_directory.joinpath(config)
            print(f'building for CMake configuration \'{config}\'')
            self._run([
                'cmake',
                '--build', str(directory), 
                '--parallel', str(self._params.cores)
            ])

    def link(self: 'Routines') -> None:
        if 'Debug' in self._params.build_configs:
            path = self._params.build_directory.joinpath('Debug').joinpath('compile_commands.json')
            print(f'creating symlink for path \'{path}\'')
        if 'Release' in self._params.build_configs:
            path = self._params.build_directory.joinpath('Release').joinpath('compile_commands.json')
            print(f'creating symlink for path \'{path}\'')
        if path is None:
            sys.exit('no supported CMake configs detected')
        pathlib.Path.cwd().joinpath('compile_commands.json').symlink_to(path)

    def _run(self: 'Routines', command: typing.List[str]) -> None:
        print('running command: ' + ' '.join(command))
        code = subprocess.run(command, encoding='UTF-8', stderr=subprocess.STDOUT, env=os.environ).returncode
        if code:
            sys.exit(f'error: subprocess failed: {errno.errorcode[code]} (code: {code})')

    def _decorate_cmake_variable(self: 'Routines', var: str, value: str, type: typing.Union[str, None] = None) -> str:
        if type is not None:
            return f'-D{var.upper()}:{type}={value}'
        return f'-D{var.upper()}={value}'


def parse_cli_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    # Routines
    parser.add_argument('-b', '--build', action='store_true', dest='build')
    parser.add_argument('-c', '--configure', action='store_true', dest='configure')
    parser.add_argument('-i', '--install', action='store_true', dest='install')
    parser.add_argument('-r', '--remove', action='store_true', dest='remove')
    parser.add_argument('-l', '--link', action='store_true', dest='link')
    # Environment
    parser.add_argument('--build-dir', action='store', dest='build_directory')
    parser.add_argument('--config', action='append', dest='configs', choices=['Debug', 'Release'])
    parser.add_argument('--parallel', action='store', dest='cores')
    parser.add_argument('--profile', action='store', dest='profile')
    return parser.parse_args()


def main() -> None:
    args = parse_cli_args()

    params = Config()
    if getattr(args, 'build_directory') is not None:
        params.build_directory = pathlib.Path.cwd().joinpath(args.directory)
        print(f'config: user-provided build directory: \'{params.build_directory}\'')
    if getattr(args, 'configs') is not None:
        params.build_configs = args.configs
        print(f'config: user-provided build configs: \'{params.build_directory}\'')
    if getattr(args, 'cores') is not None:
        params.cores = int(args.cores)
        print(f'config: user-provided threads, that will be run in parallel: \'{params.cores}\'')
    if getattr(args, 'profile') is not None:
        params.profile = args.profile
        print(f'config: user-provided conan profile: \'{params.profile}\'')

    r = Routines(params)
    for routine, f in r.routines():
        if not hasattr(args, routine):
            print(f'routine \'{routine}\' is not configured for CLI, skipping')
            continue
        if getattr(args, routine):
            print(f'running {routine}')
            f()

    print('done!')

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print('exited by user')
