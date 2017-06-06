#!/usr/bin/env python3

from jinja2 import Environment, FileSystemLoader
import os
import re
import sys

output_file = "TraCIConstants.h"
template_file = output_file + ".in"


def is_include_guard(line):
    return line.startswith("#define TRACICONSTANTS_H")


def is_constant_complete(constant):
    return {'type', 'name', 'value'}.issubset(constant)


def try_starline(line):
    m = re.match(r"// \*+", line)
    return True if m else False


def try_block(line):
    m = re.match(r"// ([A-Z].+)$", line)
    return m.group(1) if m else None


def try_constant_description(line):
    m = re.match(r"// ([^\*].+)$", line)
    return {'description': m.group(1)} if m else {}


def try_constant_definition(line):
    m = re.match(r"#define ([0-9A-Z_]+) (-?0x[0-9a-fA-F]{2})$", line)
    if m:
        return {'type': 'ubyte', 'name': m.group(1), 'value': m.group(2)}

    m = re.match(r"#define ([0-9A-Z_]+) (-?[0-9]+)$", line)
    if m:
        return {'type': 'integer', 'name': m.group(1), 'value': m.group(2)}

    return {}


def parse_constants(input_file):
    blocks = []
    block = None
    constants = []
    constant = {}
    include_guard = False
    block_level = 0

    with open(input_file, "r") as f:
        for line in f:
            if not include_guard:
                if is_include_guard(line):
                    include_guard = True
                    continue
            else:
                if block_level == 0 and try_starline(line):
                    block_level = 1
                    continue
                elif block_level == 1:
                    new_block = try_block(line)
                    if new_block:
                        if constants:
                            blocks.append({'name': block, 'constants': constants})
                            constants = []
                        block = new_block
                        block_level = 2
                    else:
                        block_level = 0
                    continue
                elif block_level == 2 and try_starline(line):
                    block_level = 0
                    continue

                defn = try_constant_definition(line)
                constant.update(defn)

                desc = try_constant_description(line)
                if desc:
                    constant.update(desc)
                    continue
                elif not is_constant_complete(constant):
                    constant = {}

                if is_constant_complete(constant):
                    constants.append(constant)
                    constant = {}
                    continue

        if constants:
            blocks.append({'name': block, 'constants': constants})

    return blocks


def write_output(blocks):
    path = os.path.dirname(os.path.abspath(__file__))
    environment = Environment(loader=FileSystemLoader(path))
    template = environment.get_template(template_file)
    result = template.render(blocks=blocks)
    with open(output_file, "w") as f:
        f.write(result)


def usage(prog):
    print("Usage: {} <TraCIConstants header from SUMO>".format(prog))


if __name__ == "__main__":
    if len(sys.argv) != 2:
        usage(sys.argv[0])
        exit(1)
    else:
        blocks = parse_constants(sys.argv[1])
        write_output(blocks)
        print("{} + {} -> {}".format(sys.argv[1], template_file, output_file))
