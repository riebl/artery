#!/usr/bin/env python3

import sys
import copy
import argparse
import subprocess
import configparser

from pathlib import Path


for template, value in copy.copy(globals()).items():
    if not template.startswith('OPP'):
        continue
    if value.startswith('@') or value.endswith('@'):
        raise ValueError(
            f'template variable {template} is left unitialized: {value}'
        )


def main():
    global OPP_RUNALL, OPP_RUN, OPP_DEBUG_RUN, \
        OPP_RUN_NED_FOLDERS, OPP_RUN_LIBRARIES

    parser = argparse.ArgumentParser()

    parser.add_argument('--all', action='store_true', help='sets runall mode')
    parser.add_argument('-b', '--batchsize', dest='batch', action='store')
    parser.add_argument('-j', '--jobs', dest='jobs', action='store')
    parser.add_argument('--artery-launch-conf', action='store')

    args, unrecognized = parser.parse_known_args()

    cmd = []
    working_directory = Path.cwd()

    if args.artery_launch_conf is None:
        raise RuntimeError(
            'missing artery launch config, which should be under build/'
        )

    config_parser = configparser.ConfigParser(default_section='General')
    with open(Path(args.artery_launch_conf)) as config_file:
        config_parser.read_file(config_file)

    if (opp_run := config_parser.get('General', 'oppRun', fallback=None)) is None:
        raise ValueError('missing oppRun in General section')
    if (opp_runall := config_parser.get('General', 'oppRunAll', fallback=None)) is None:
        raise ValueError('missing oppRunAll in General section')
    if (ned_folders := config_parser.get('General', 'nedFolders', fallback=None)) is None:
        raise ValueError('missing nedFolders in General section')
    if (libraries := config_parser.get('General', 'libraries', fallback=None)) is None:
        raise ValueError('missing libraries in General section')

    if args.all:
        cmd.append(opp_runall)
        if args.batch is not None:
            cmd.extend(['-b', args.batch])
        if args.jobs is not None:
            cmd.extend(['-j', args.jobs])

    cmd.append(opp_run)
    cmd.extend(['-n', ned_folders])
    cmd.extend(libraries.split())
    cmd.extend(unrecognized)

    print('running command: ', ' '.join(cmd))

    process = subprocess.run(cmd, cwd=working_directory, stderr=sys.stderr, stdout=sys.stdout)
    sys.exit(process.returncode)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print('exited by user')
