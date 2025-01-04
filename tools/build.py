#!/bin/env python3

import os
import sys
import errno
import shutil
import typing
import pathlib
import tarfile
import argparse
import fileinput
import subprocess
import multiprocessing

DEFAULT_BUILD_DIR = 'build'

DEFAULT_CONAN_DIR = 'conan2'
DEFAULT_OMNETPP_DIR = 'omnetpp'

DEFAULT_PROFILE_PATH = 'tools/profiles/default.ini'
DEFAULT_OMNETPP_TAG = 'omnetpp-5.6.2'
DEFAULT_OMNETPP_URL = 'https://github.com/omnetpp/omnetpp/releases/download/{0}/{0}-src-linux.tgz'
DEFAULT_SUMO_HOME = '/usr/local/share/sumo' 

DEFAULT_BUILD_CONFIGS = ['Debug']

usage = """
Usage

    CLI builder is used to set up build environment, check all 
    dependencies and run CMake build/configure routines.

    This version has added functionality to handle Artery deps specifically.

    -b --build              run CMake build routine. Requires configured environment.
    -c --configure          configure CMake and check environment utils.
    -i --install            perform installation of components
    -f --force              remove everything, even local conan and omnetpp installs.
       --dir                specify build directory
       --config CONFIG      run for specified config. (Release, Debug) Default - only Debug.
                            You might invoke this multiple times.
       --link               create symlink in project root to Debug compile commands.
       --omnetpp-tag TAG    tag for omnetpp local install, please refer to omnetpp download page
                            for further reference.
       --profile            profile to use with conan. Make sure to adjust it for local needs, and
                            add to tools/profiles/ for others to refer to.
    -c --clear              clears build directory, when used with other routines, does that before running them.
       --local-conan        installs conan to deps directory, useful in packaged builds. 

"""

class Routines:

    # Installs local deps
    @staticmethod
    def install(args: argparse.Namespace) -> None:
        directory, _ = get_common(args)
        source_file = 'source.tgz'
        cores = get_arg(args, 'cores', None)
        if not cores:
            cores = multiprocessing.cpu_count()
        cores = int(cores)
        omnetpp_dir = f'{directory}/{DEFAULT_OMNETPP_DIR}'

        # this is hardly done just to transfer all setup code to python, sooooo
        if not os.path.exists(omnetpp_dir):
            # handle install
            omnetpp_tag = get_arg(args, 'omnetpp_tag', DEFAULT_OMNETPP_TAG)
            print(f'[install] installing omnetpp: {omnetpp_tag}')
            command = ['wget', '-c', DEFAULT_OMNETPP_URL.format(omnetpp_tag), '-O', source_file]
            handle_subprocess(subprocess.run(command, cwd=directory, encoding='UTF-8', stderr=subprocess.STDOUT, env=os.environ), 'install')
            # handle unpack
            with tarfile.open(f'{directory}/{source_file}', 'r:gz') as tar:
                tar.extractall(path=omnetpp_dir)
            # handle build
            # see https://askubuntu.com/questions/1035220/error-while-installing-omnet-on-ubuntu-16-04-cannot-find-osgearth
            for line in fileinput.input(f'{omnetpp_dir}/{omnetpp_tag}/configure.user', inplace=True):
                if line.startswith('WITH_OSGEARTH='):
                    print('WITH_OSGEARTH=no')
                else:
                    print(line)
            command = f'source $PWD/setenv -f && ./configure && make -j{cores}'
            handle_subprocess(subprocess.run(command, shell=True, cwd=f'{omnetpp_dir}/{omnetpp_tag}', encoding='UTF-8', stderr=subprocess.STDOUT, env=os.environ), 'install')
        else:
            print('[install] skipping omnetpp install')

        conan_dir = f'{directory}/{DEFAULT_CONAN_DIR}'
        if not os.path.exists(conan_dir) and hasattr(args, 'local_conan'):
            print('[install] making conan local dirs')
            os.mkdir(conan_dir)
            os.mkdir(f'{conan_dir}/profiles')
        else:
            print('[install] skipping conan setup')

        # if not os.path.exists(DEFAULT_SUMO_HOME):
        #     print('[install] installing sumo')
        #     command = ['git', 'clone', '--recurse', '--depth', '1', 'https://github.com/eclipse-sumo/sumo']
        #     handle_subprocess(subprocess.run(command, cwd=directory, encoding='UTF-8', stderr=subprocess.STDOUT, env=os.environ), 'install')
        #     command = f'cmake -B build . && cmake --build build -j{cores} && cmake --install build'
        #     handle_subprocess(subprocess.run(command, shell=True, cwd=f'{directory}/sumo', encoding='UTF-8', stderr=subprocess.STDOUT, env=os.environ), 'install')
        # else:
        #     print('[install] sumo already installed')


    # Clear build files
    @staticmethod
    def clear(args: argparse.Namespace) -> None:
        directory, configs = get_common(args)

        force = get_arg(args, 'force', None)
        if force and os.path.exists(directory) and os.path.isdir(directory):
            confirm = input(f'[clear] force-removing everything in directory: {directory}. Are you sure? (N/y) ')
            if not confirm:
                confirm = 'n'
            if confirm != 'y':
                sys.exit(errno.ECANCELED)
                
            conan_dir = f'{directory}/{DEFAULT_CONAN_DIR}'
            if os.path.exists(conan_dir):
                shutil.rmtree(conan_dir)
                print(f'[clear] removed conan local install')
            else:
                print(f'[clear] conan local install does not exist, skipping')

            omnetpp_dir = f'{directory}/{DEFAULT_OMNETPP_DIR}'
            if os.path.exists(omnetpp_dir):
                shutil.rmtree(omnetpp_dir)
                print(f'[clear] removed omnetpp install')
            else:
                print(f'[clear] omnetpp install does not exist, skipping')

        for config in configs:
            dir = f'{directory}/{config.capitalize()}'
            if os.path.exists(dir):
                shutil.rmtree(dir)
                print(f'[clear] removing directory for config: {config}')
            else:
                print(f'[clear] skipping config: {config}')
            

    # Runs configure routine - part of build toolchain
    @staticmethod
    def configure(args: argparse.Namespace) -> None:
        directory, configs = get_common(args)

        if not os.path.exists(directory):
            os.mkdir(directory)  

        print('[configure] configuring for CMake build configs: ' + ', '.join(configs))
        conan_args = []

        profile = get_arg(args, 'profile', DEFAULT_PROFILE_PATH)
        # stem = pathlib.PurePath(profile).stem

        # target = f'{directory}/{DEFAULT_CONAN_DIR}/profiles/{stem}'
        # if not os.path.exists(profile):
        #     print(f'[configure] failed to locate profile: {profile}')
        #     sys.exit(errno.ENOENT)
        # if os.path.exists(target):
        #     print(f'[configure] warning: profile {stem} already exists in destination, overwriting')
        #     os.remove(target)
        # shutil.copy(profile, target)

        print(f'[configure] using conan profile: {profile}')
        conan_args.append(f'-pr:a={profile}')

        print(f'[configure] running conan')
        command = ['conan', 'install', os.curdir, '--build=missing', *conan_args]
        handle_subprocess(subprocess.run(command, encoding='UTF-8', stderr=subprocess.STDOUT, env=os.environ), 'configure')

        for config in configs:
            print(f'[configure] running configure command for config {config}')
            command = ['cmake', '--preset', f'conan-{config.lower()}', '-B', f'{directory}/{config}', '-S', f'{os.curdir}', f'-DCMAKE_BUILD_TYPE={config}']
            handle_subprocess(subprocess.run(command, encoding='UTF-8', stderr=subprocess.STDOUT, env=os.environ), 'configure')

    # Runs actual build routine
    @staticmethod
    def build(args: argparse.Namespace) -> None:        
        directory, configs = get_common(args)

        print(f'[build] using directory: {directory}')
        if not os.path.exists(directory):
            print('[build] build directory not found.')
            sys.exit(errno.ENOENT)

        cores = get_arg(args, 'cores', None)
        if not cores:
            cores = multiprocessing.cpu_count()
        cores = int(cores)
        print(f'[build] running build with {cores} threads')
        
        for config in configs:
            print(f'[build] running build command for config {config}...')
            command = ['cmake', '--build', f'{directory}/{config}', '-j', str(cores), '--preset', f'conan-{config.lower()}']
            handle_subprocess(subprocess.run(command, encoding='UTF-8', stderr=subprocess.STDOUT, env=os.environ), 'build')

# Wraps subprocess execution, allows to exit gracefully on error.
def handle_subprocess(process: subprocess.CompletedProcess, scope: str) -> None:
    retval = process.returncode
    if retval != 0:
        print(f'[{scope}] error: subroutine failed (code: {retval}): {errno.errorcode[retval]}')
        sys.exit(retval)

# Wraps cached cmake vars into cmake-friendly syntax
def cmake_var_decorator(var: str, value: str, type: str = 'STRING') -> str:
    return f'-D{var.upper()}:{type}={value}'

# Runs checks for executables specified by names.
def utility_check(names: typing.List[str], scope: str) -> None:
    for name in names:
        print(f'[{scope}] checking utility: {name}, running $PATH check...', end=' ')
        locations = subprocess.run(['whereis', name], stdout=subprocess.PIPE, encoding='UTF-8').stdout.split()[1:]
        if not locations:
            print('failed')
            sys.exit(errno.ENOENT)
        print('ok', f'[{scope}] found utility in: ' + ', '.join(locations), sep='\n', end='\n')

def get_arg(args: argparse.Namespace, arg: str, default: typing.Any) -> typing.Any:
    if hasattr(args, arg):
        if getattr(args, arg) is not None:
            return getattr(args, arg)
    return default

# symlinks compilation commands to assist Clang in helping you code
def link_compile_commands(path: str) -> None:
    if os.path.exists('compile_commands.json'):
        print('[misc] compile commands file exists, overwriting')
        os.remove('compile_commands.json')
    os.symlink(path, 'compile_commands.json')
    print('[misc] symlink created')

# parse CLI args into a namespace
def parse_cli_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        add_help=False
    )
    parser.add_argument('-h', '--help', action='store_true', dest='help')
    parser.add_argument('-b', '--build', action='store_true', dest='build')
    parser.add_argument('-c', '--configure', action='store_true', dest='configure')
    parser.add_argument('-i', '--install', action='store_true', dest='install')
    parser.add_argument('-f', '--force', action='store_true', dest='force')
    parser.add_argument('-r', '--clear', action='store_true', dest='clear')
    parser.add_argument('--dir', action='store', dest='directory')
    parser.add_argument('--config', action='append', dest='config')
    parser.add_argument('--link', action='store_true', dest='link')
    parser.add_argument('-j', action='store', dest='cores')

    # Artery-specific setup
    parser.add_argument('--omnetpp-tag', action='store', dest='omnetpp_tag')
    parser.add_argument('--profile', action='store', dest='profile')
    parser.add_argument('--local-conan', action='store_true', dest='local_conan')

    return parser.parse_args()

# return common args, used almost everywhere
def get_common(args: argparse.Namespace) -> typing.List[typing.Tuple[str, typing.List[str]]]:
    directory = get_arg(args, 'directory', DEFAULT_BUILD_DIR)
    configs = get_arg(args, 'config', DEFAULT_BUILD_CONFIGS)
    return directory, configs


if __name__ == '__main__':
    args = parse_cli_args()

    if getattr(args, 'help'):
        print(usage)
        sys.exit()

    requires = ['cmake', 'gcc', 'conan', 'wget', 'make']
    print('[buildsystem] running build. checking requirements: ' + ', '.join(requires))
    utility_check(requires, scope='buildsystem')

    directory, configs = get_common(args)
    if not os.path.exists(directory):
        os.mkdir(directory)
    
    # setup env to use later in routines
    os.environ['SUMO_HOME'] = DEFAULT_SUMO_HOME
    if hasattr(args, 'local_conan'):
        os.environ['CONAN_HOME'] = os.path.abspath(f'{directory}/{DEFAULT_CONAN_DIR}')
    os.environ['PATH'] = '{0}:{1}/{2}/{3}/bin'.format(
        os.environ['PATH'], 
        os.path.abspath(directory), 
        DEFAULT_OMNETPP_DIR,
        get_arg(args, 'omnetpp_tag', DEFAULT_OMNETPP_TAG)
    )
    print(os.environ['PATH'])

    if getattr(args, 'clear'):
        print('[buildsystem] invoking clear routine')
        Routines.clear(args)

    if getattr(args, 'install'):
        print('[buildsystem] invoking install routine')
        Routines.install(args)

    if getattr(args, 'configure'):
        print('invoking configure routine')
        Routines.configure(args)

    if getattr(args, 'build'):
        print('invoking build routine')
        Routines.build(args)

    if getattr(args, 'link'):
        if 'Debug' in configs:
            link_compile_commands(directory + '/Debug/compile_commands.json')
        else:
            link_compile_commands(directory + '/Release/compile_commands.json')

    print('finished!')
