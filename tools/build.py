#!/usr/bin/env python3

import os
import sys
import errno
import shutil
import typing
import inspect
import pathlib
import argparse
import subprocess
import multiprocessing

from dataclasses import dataclass, field

description = """
This script primarily helps running cmake configure & build commands, but can also
handle compile commands export, build artefact removal and conan setup. All of those
things can be done just by adding an extra flag to build.py command, which is convenient.

All functionality is splitted into routines that correspond to specific task you wish to accomplish.
The basic routines are configure and build, they can be invoked with -c (--configure) and -b (--build)
or -cb for short. The order of arguments does not matter. Examples:

Running configure & build using default \'build\' directory:
./build.py -cb

Same as above, but using different directory:
./build.py -cb --build-dir my_build_directory

Removing subdirectories for \'Release\' and \'Debug\' configs:
./build.py -r --config Release --config Debug

Same as above, but performing configure & build afterwords (for both configs):
./build.py -rcb --config Release --config Debug

Generating conan toolchain:
./build.py -i --config Debug

Creating a symlink to build/.../compile_commands.json:
./build.py -l --config Debug
"""


@dataclass
class Config:
    build_directory: pathlib.Path = field(default_factory=lambda: pathlib.Path.cwd().joinpath('build'))
    build_configs: typing.List[str] = field(default_factory=lambda: ['Debug'])
    cores: int = multiprocessing.cpu_count()
    profile: typing.Union[pathlib.Path, str] = 'default'
    generator: str = 'Ninja'


def routine(priority: int) -> typing.Callable:
    def decorator(method):
        method.priority = priority
        method.is_routine = True
        return method
    return decorator


class Routines:

    def __init__(self: 'Routines', params: Config) -> None:
        self._params = params

    def routines(self: 'Routines') -> typing.Generator[typing.Tuple[str, typing.Callable], None, None]:
        def key(pair):
            _, member = pair
            return member.priority if hasattr(member, 'priority') else 0

        members = inspect.getmembers(self, predicate=inspect.ismethod)
        for name, method in sorted(members, key=key, reverse=True):
            if hasattr(method, 'is_routine'):
                yield name, method

    @routine(5)
    def remove(self: 'Routines') -> None:
        for config in self._params.build_configs:
            directory = self._params.build_directory.joinpath(config)
            if directory.is_dir():
                print(f'removing directory for CMake build config \'{config}\'')
                shutil.rmtree(directory)
            else:
                print(f'build directory for config \'{config}\' was not found or was not a directory')

    @routine(4)
    def conan_install(self: 'Routines') -> None:
        for config in self._params.build_configs:
            print(f'running conan install command for CMake config {config}')
            self._run([
                'conan',
                'install',
                '--build=missing',
                f'-pr:a={self._params.profile}',
                self._decorate_conan_profile_entry('settings', 'build_type', config),
                self._decorate_conan_profile_entry('conf', 'user.recipe:build_dir', self._params.build_directory),
                str(pathlib.Path.cwd().joinpath('devcontainers'))
            ])

    @routine(3)
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
                '-G', self._params.generator,
                '-B', str(binary),
                '-S', str(source),
                self._decorate_cmake_variable('CMAKE_EXPORT_COMPILE_COMMANDS', 'ON', 'BOOL'),
                self._decorate_cmake_variable('CMAKE_BUILD_TYPE', config)
            ] if use_presets else [
                'cmake',
                '-G', self._params.generator,
                '-B', str(binary),
                '-S', str(source),
                self._decorate_cmake_variable('CMAKE_EXPORT_COMPILE_COMMANDS', 'ON', 'BOOL'),
                self._decorate_cmake_variable('CMAKE_BUILD_TYPE', config)
            ]
            print(f'running configure command for CMake config {config}')
            self._run(command)

    @routine(2)
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

    @routine(1)
    def symlink_compile_commands(self: 'Routines') -> None:
        if 'Debug' in self._params.build_configs:
            path = self._params.build_directory.joinpath('Debug').joinpath('compile_commands.json')
        if 'Release' in self._params.build_configs:
            path = self._params.build_directory.joinpath('Release').joinpath('compile_commands.json')
        if path is None:
            sys.exit('no supported CMake configs detected')
        print(f'creating symlink for path \'{path}\'')
        symlink = pathlib.Path.cwd().joinpath('compile_commands.json')
        if symlink.is_symlink():
            symlink.unlink()
        pathlib.Path.cwd().joinpath('compile_commands.json').symlink_to(path)

    def _run(self: 'Routines', command: typing.List[str]) -> None:
        print('running command: ' + ' '.join(command))
        retval = subprocess.run(command, encoding='UTF-8', stderr=subprocess.STDOUT, env=os.environ).returncode
        if retval:
            sys.exit(f'error: subprocess failed: {errno.errorcode[retval]} (code: {retval})')

    def _decorate_cmake_variable(self: 'Routines', var: str, value: str, type: typing.Union[str, None] = None) -> str:
        if type is not None:
            return f'-D{var.upper()}:{type}={value}'
        return f'-D{var.upper()}={value}'

    def _decorate_conan_profile_entry(self: 'Routines', section: str, entry: str, value: str) -> str:
        return f'--{section}={entry}={value}'


def parse_cli_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=description, formatter_class=argparse.RawDescriptionHelpFormatter)
    # Routines
    parser.add_argument('-b', '--build', action='store_true', dest='build',
        help='runs cmake build, runs after configure.'
    )
    parser.add_argument('-c', '--configure', action='store_true', dest='configure',
        help='runs cmake configure. If CMakeUserPresets.json is preset, uses conan presets. '
        'Runs after install.'
    )
    parser.add_argument('-i', '--conan-install', action='store_true', dest='conan_install',
        help='runs conan install with devcontainers/conanfile.py. If you need specific profile, '
        '(other than default) specify it with --profile, some examples are located under '
        'devcontainers/templates as .ini files.'
    )
    parser.add_argument('-r', '--remove', action='store_true', dest='remove',
        help='removes subdirectories in build/, according to configs provided. Might be useful '
        'if CMake fresh configuration is required. Always executed before any other routine.'
    )
    parser.add_argument('-l', '--symlink-compile-commands', action='store_true', dest='symlink_compile_commands',
        help='creates symlink to build/.../compile_commands.json in source directory. In case '
        'both configs specified, links to Debug. In case symlink already exists, rewrites it.'
    )
    # Environment
    parser.add_argument('--build-dir', action='store', dest='build_directory',
        help='specify root directory to put build files in. Defaults to \'build\'.'
    )
    parser.add_argument('--config', action='append', dest='configs', choices=['Debug', 'Release'],
        help='specify configs for build, options are Debug and Release.'
    )
    parser.add_argument('--parallel', action='store', dest='cores',
        help='similar to --parallel in cmake, specify how many threads will handle build.'
    )
    parser.add_argument('--generator', action='store', dest='generator',
        help='specify CMake generator to use.'
    )
    parser.add_argument('--profile', action='store', dest='profile',
        help='specify path to profile, or its name if available.'
    )
    return parser.parse_args()


def main() -> None:
    args = parse_cli_args()

    params = Config()
    if getattr(args, 'build_directory') is not None:
        params.build_directory = pathlib.Path.cwd().joinpath(args.build_directory)
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
    if getattr(args, 'generator') is not None:
        params.generator = args.generator
        print(f'config: user-provided generator: \'{params.generator}\'')

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
