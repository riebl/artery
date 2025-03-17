#!/usr/bin/env python3
import sys
import argparse
import subprocess
import configparser
import pathlib


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--all', action='store_true', help='sets runall mode')
    parser.add_argument('-b', '--batchsize', dest='batch', action='store')
    parser.add_argument('-j', '--jobs', dest='jobs', action='store')
    parser.add_argument('-l', '--launch-conf', action='store', required=True, type=pathlib.Path)
    parser.add_argument('-s', '--scenario', default=pathlib.Path.cwd(), type=pathlib.Path)
    parser.add_argument('-v', '--verbose', action='store_true')
    args, opp_args = parser.parse_known_args()

    # remove '--' from opp_args when used to split run_artery args from opp_run args
    if len(opp_args) > 0 and opp_args[0] == '--':
        opp_args = opp_args[1:]
    
    if args.launch_conf.is_file():
        config_filename = args.launch_conf
    elif args.launch_conf.is_dir():
        config_filename = args.launch_conf / 'run-artery.ini'
    else:
        raise ValueError('Argument "launch-conf" must be an existing file or directory')

    config_parser = configparser.ConfigParser(default_section='General')
    with open(config_filename) as config_file:
        config_parser.read_file(config_file)

    if (opp_run := config_parser.get('Executables', 'oppRun', fallback=None)) is None:
        raise ValueError('missing oppRun in Executables section')
    if (opp_runall := config_parser.get('Executables', 'oppRunAll', fallback=None)) is None:
        raise ValueError('missing oppRunAll in Executables section')
    if (ned_folders := config_parser.get('Artery', 'nedFolders', fallback=None)) is None:
        raise ValueError('missing nedFolders in Artery section')
    if (libraries := config_parser.get('Artery', 'libraries', fallback=None)) is None:
        raise ValueError('missing libraries in Artery section')

    cmd = []
    if args.all:
        cmd.append(opp_runall)
        if args.batch is not None:
            cmd.extend(['-b', args.batch])
        if args.jobs is not None:
            cmd.extend(['-j', args.jobs])

    cmd.append(opp_run)
    cmd.extend(['-n', ned_folders])
    cmd.extend(libraries.split())
    cmd.extend(opp_args)

    if args.verbose:
        print('running command: ', ' '.join(cmd))

    process = subprocess.run(cmd, cwd=args.scenario, stderr=sys.stderr, stdout=sys.stdout)
    sys.exit(process.returncode)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print('exited by user')
