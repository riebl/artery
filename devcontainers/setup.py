#!/usr/bin/env python3

import os
import sys
import json
import typing
import pathlib
import argparse

description = """
This utility script helps you setting up development environment inside docker container
(called DevContainer), which is quite convenient for versioning and well supported by VSCode.

Generally you only provide setup name, which is displayed as config name in VSCode (-s, --setup-name), and
distribution which you wish to work with. You can also specify output json filepath, individual component versions
(the ones compiled in container, anyways) and user name to use inside the container.

Before setting versions manually, check github repos of respective component for correct tag.

Example:
./devcontainers/setup.py -s example-config -d arch-linux --output .devcontainer/example.json
"""

DEVCONTAINER_CONFIG_PATH = '.devcontainer/devcontainer.json'


def get_distributions(template_path: pathlib.Path) -> typing.List[str]:
    collected = []
    for entity in template_path.iterdir():
        if not entity.is_dir():
            continue
        directory = pathlib.Path(entity)
        files = filter(lambda entity: entity.is_file(), directory.iterdir())
        if 'Dockerfile' in map(lambda file: file.stem, files):
            collected.append(directory.stem)
    return collected


def parse_cli_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=description, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-s', '--setup-name', action='store', dest='setup_name', required=True,
        help='setup name, which is displayed in GUI. You can choose whatever suits you.'    
    )
    dists = get_distributions(pathlib.Path('devcontainers/templates'))
    parser.add_argument('-d', '--dist', action='store', dest='dist', choices=dists, required=True,
        help='distribution name, located under devcontainers/templates/.' 
    )
    parser.add_argument('--omnetpp-version', action='store', dest='omnetpp_version', default='omnetpp-5.6.2',
        help='version for Omnetpp++.'
    )
    parser.add_argument('--sumo-version', action='store', dest='sumo_version', default='v1_21_0',
        help='version for sumo.'
    )
    parser.add_argument('--geographiclib-version', action='store', dest='geographiclib_version', default='v2.5',
        help='version for geographiclib, arch-linux image compiles this.'
    )
    parser.add_argument('--user', action='store', dest='user', default='devcontainer',
        help='user name to use inside the container.'
    )
    parser.add_argument('--tag', action='store', dest='tag', default=None,
        help='tag to use for distribution, like bookworm for debian of base-devel for arch.'
    )
    parser.add_argument('--output', action='store', dest='output', default=DEVCONTAINER_CONFIG_PATH,
        help='path of output file, defaults to standard config name for VSCode.'
    )
    return parser.parse_args()


def main() -> None:
    args = parse_cli_args()

    output = pathlib.Path(args.output)
    if output.is_file():
        sys.exit(f'output file: \'{output}\' already exists, delete it first or choose different output location')

    if 'DISPLAY' not in os.environ:
        print(
            'warning: cannot find DISPLAY environment variable. This script does not require it, '
            'but container needs it to connect to Xorg server and display OmnetPP window in host environment'
        )

    if 'XAUTHORITY' not in os.environ:
        print(
            'warning: cannot find XAUTHORITY environment variable. This script does not require it, '
            'but container might need this file to connect to Xorg'
        )

    tag = args.tag
    if tag is not None:
        print(f'- using distribution: \'{args.dist}:{tag}\' with user \'{args.user}\'')
    else:
        print(f'- using distribution: \'{args.dist}\' with user \'{args.user}\'')
    for component in ['omnetpp', 'sumo', 'geographiclib']:
        print(f'-- using component \'{component}\' version \'' + getattr(args, f'{component}_version') + '\'')


    config = {
        'name': args.setup_name,
        'build': {
            'context': '..',
            'dockerfile': f'../devcontainers/templates/{args.dist}/Dockerfile',
            'args': {
                'USER': args.user,
                'OMNETPP_TAG': args.omnetpp_version,
                'SUMO_TAG': args.sumo_version,
                'GEOGRAPHICLIB_TAG': args.geographiclib_version
            }
        },
        'containerUser': args.user,
        'containerEnv': {
            # required to run artery inside the container
            'DISPLAY': r'${localEnv:DISPLAY}',
            # optional; xorg connections might be configured manually in host environment 
            'XAUTHORITY': r'${localEnv:XAUTHORITY}'
        },
        'mounts': [
            {
                'source': r'${localEnv:XAUTHORITY}', 
                'target': r'${localEnv:XAUTHORITY}', 
                'type': 'bind' 
            }, { 
                'source': '/tmp/.X11-unix', 
                'target': '/tmp/.X11-unix', 
                'type': 'bind' 
            }
        ]
    }

    if tag is not None:
        config['build']['args']['TAG'] = tag

    with open(output, 'w') as file:
        json.dump(config, file, indent=4)
        print(f'wrote config to a file: \'{output}\'')
        print(f'you now should be able to pick it in your Editor of choice, name is {args.setup_name}')


if __name__ == '__main__':
    main()
