#!/usr/bin/env python3

from functools import partial
import os
import re
import subprocess
import sys


class OmnetProject:

    def __init__(self, makefile):
        self.name = ""
        self.makefile = makefile
        self.makefile_defines = {}
        self.root_directory = self.lookup_project_directory()
        self.output_directory = "out"
        self.include_directories = []
        self.include_directories_deep = []
        self.library_directories = []
        self.link_libraries = []
        self.compile_definitions = []
        self.binary = "executable"
        self.ned_folders = []

    def lookup_project_directory(self):
        candidate = os.path.dirname(self.makefile)
        root = os.path.abspath(os.sep)
        while (os.path.isfile(os.path.join(candidate, '.project')) is False):
            candidate = os.path.abspath(os.path.join(candidate, os.path.pardir))
            if (candidate == root):
                raise ValueError("Can not determine project directory for ${self.makefile}")
        return candidate

    def read_ned_folders(self):
        self.ned_folders = []
        with open(os.path.join(self.root_directory, '.nedfolders'), 'r') as dotnedfolders:
            for line in dotnedfolders:
                if line[0] != '-':
                    self.ned_folders.append(line.strip())


class CMakeTarget:

    def __init__(self, project, toolchain='gcc'):

        self._project = project
        self._toolchain = toolchain
        self.include_directories = [self._make_absolute_path(self._makefile_path(), d) for d in self._project.include_directories]
        for dd in self._project.include_directories_deep:
            self.include_directories.append(dd)
        self.library_directories = [self._make_absolute_path(self._makefile_path(), d) for d in self._project.library_directories]
        self.ned_folders = [self._make_absolute_path(self.root_directory, d) for d in self._project.ned_folders]

    def _make_absolute_path(self, root, path):
        return os.path.normpath(os.path.join(root, path))

    def _makefile_path(self):
        return os.path.dirname(self._project.makefile)

    def _target_output_path(self, mode):
        out_subdir = os.path.relpath(self._makefile_path(), self._project.root_directory)
        return os.path.join(self._project.root_directory, self._project.output_directory, mode, out_subdir)

    def _filename(self, mode):
        binary = {
            'executable': "{}${{CMAKE_EXECUTABLE_SUFFIX}}",
            'shared': "${{CMAKE_SHARED_LIBRARY_PREFIX}}{}${{CMAKE_SHARED_LIBRARY_SUFFIX}}",
            'static': "${{CMAKE_STATIC_LIBRARY_PREFIX}}{}${{CMAKE_STATIC_LIBRARY_SUFFIX}}"
        }
        return binary[self._project.binary].format(self._project.name)

    def _import_location(self, mode):
        filename = self._filename(mode)
        path = None
        if (mode == "release"):
            path = self._target_output_path('{}-release'.format(self._toolchain))
        elif (mode == "debug"):
            path = self._target_output_path('{}-debug'.format(self._toolchain))
        else:
            path = self._makefile_path()
        return os.path.join(path, filename)

    def __getattr__(self, name):
        return getattr(self._project, name)

    @property
    def location(self):
        return self._import_location()

    @property
    def location_debug(self):
        return self._import_location('debug')

    @property
    def location_release(self):
        return self._import_location('release')

    @property
    def target(self):
        target = {
            'executable': "add_executable({} IMPORTED GLOBAL)",
            'shared': "add_library({} SHARED IMPORTED GLOBAL)",
            'static': "add_library({} STATIC IMPORTED GLOBAL)"
        }

        return target[self._project.binary].format(self._project.name)

    @property
    def target_properties(self, ned_folders_property=True):
        props = ["set_target_properties({} PROPERTIES".format(self.name)]
        configurations = []
        if os.path.exists(os.path.dirname(self.location_release)):
            props.append("  IMPORTED_LOCATION_RELEASE \"{}\"".format(self.location_release))
            props.append("  IMPORTED_NO_SONAME TRUE")
            configurations.append("RELEASE")
        if os.path.exists(os.path.dirname(self.location_debug)):
            props.append("  IMPORTED_LOCATION_DEBUG \"{}\"".format(self.location_debug))
            props.append("  IMPORTED_NO_SONAME TRUE")
            configurations.append("DEBUG")
        if self.binary != "executable":
            if self.include_directories:
                include_dirs = ';'.join(self.include_directories)
                props.append("  INTERFACE_INCLUDE_DIRECTORIES \"{}\"".format(include_dirs))
            if self.link_libraries:
                link_libraries = ';'.join(self.link_libraries)
                props.append("  INTERFACE_LINK_LIBRARIES \"{}\"".format(link_libraries))
            if self.compile_definitions:
                compile_defs = ';'.join(self.compile_definitions)
                props.append("  INTERFACE_COMPILE_DEFINITIONS \"{}\"".format(compile_defs))
            if ned_folders_property and self.ned_folders:
                ned_folders = ';'.join(self.ned_folders)
                props.append("  NED_FOLDERS \"{}\"".format(ned_folders))
        props.append(")")
        props.append("set_property(TARGET {target} PROPERTY IMPORTED_CONFIGURATIONS {configs})"
                     .format(target=self.name, configs=" ".join(configurations)))
        return props


class FlagsHandler:

    def __init__(self, makefile, configfile):
        self._iterator = None
        self._options = {
            "-f": self.ignore,
            "--force": self.ignore,
            "-p": self.skip,
            "--deep": self.ignore,
            "--no-deep-includes": self.ignore,
            "-s": partial(self.binary_type, "shared"),
            "--make-so": partial(self.binary_type, "shared"),
            "-a": partial(self.binary_type, "static"),
            "--make-lib": partial(self.binary_type, "static"),
            "-o": self.project_name,
            "-O": self.output_directory,
            "--out": self.output_directory,
            "-X": self.exclude_directory,
            "--except": self.exclude_directory,
            "-I": self.include_directory,
            "-L": self.library_directory,
            "-l": self.library,
            "-D": self.compile_definition,
            "--define": self.compile_definition,
            "-P": self.project_directory,
            "--projectdir": self.project_directory,
            "-K": self.makefile_define,
            "--makefile-define": self.makefile_define
        }
        self._project = OmnetProject(makefile)
        self._configfile = configfile

    def process(self, flags):
        self._iterator = iter(flags)
        for flag in self._iterator:
            if flag in self._options:
                self._options[flag]()
            else:
                raise RuntimeError("Unknown flag", flag)

        for key, value in self._project.makefile_defines.items():
            key_pattern = "$(" + key + ")"
            self._project.include_directories = [incdir.replace(key_pattern, value) for incdir
                                                 in self._project.include_directories]
            self._project.library_directories = [libdir.replace(key_pattern, value) for libdir
                                                 in self._project.library_directories]
            self._project.link_libraries = [lnklib.replace(key_pattern, value) for lnklib
                                            in self._project.link_libraries]

    def target(self):
        self._project.read_ned_folders()
        return CMakeTarget(self._project, which_opp_toolchain(self._configfile))

    def ignore(self):
        pass

    def skip(self):
        next(self._iterator)

    def fetch(self):
        return next(self._iterator)

    def binary_type(self, binary):
        self._project.binary = binary

    def project_name(self):
        self._project.name = self.fetch()

    def output_directory(self):
        self._project.output_directory = self.fetch()

    def exclude_directory(self):
        self.skip()

    def include_directory(self):
        self._project.include_directories.append(self.fetch())

    def library_directory(self):
        self._project.library_directories.append(self.fetch())

    def library(self):
        self._project.link_libraries.append(self.fetch())

    def compile_definition(self):
        self._project.compile_definitions.append(self.fetch())

    def project_directory(self):
        self._project.root_directory = self.fetch()

    def makefile_define(self):
        (name, value) = self.fetch().split('=', maxsplit=1)
        self._project.makefile_defines[name] = value


def find_opp_configfile():
    configfile = None

    if os.environ.get('OMNETPP_CONFIGFILE'):
        configfile = os.environ['OMNETPP_CONFIGFILE']
    elif os.environ.get('OMNETPP_ROOT'):
        configfile = os.path.join(os.environ['OMNETPP_ROOT'], 'Makefile.inc')
    else:
        opp_configfilepath = subprocess.check_output('opp_configfilepath', shell=True)
        configfile = str(opp_configfilepath, encoding='utf-8').rstrip('\n')

    if not os.path.isfile(configfile):
        raise Exception('Assumed OMNeT++ configfile at {} does not exist'.format(configfile))

    return configfile


def which_opp_toolchain(configfile_path):
    with open(configfile_path, "r", encoding="utf-8") as configfile:
        toolchain_pattern = re.compile("TOOLCHAIN_NAME = (\w+)")
        for line in configfile:
            line_match = toolchain_pattern.match(line)
            if line_match:
                return line_match.group(1)


def parse_opp_makefile(makefile):
    command = extract_makemake_command(makefile)
    makemake_flags = command.split()
    makemake_flags.pop(0)
    # Normalize flags
    for i, flag in enumerate(makemake_flags):
        if len(flag) > 2 and flag.startswith("-") and not flag.startswith("--"):
            makemake_flags.insert(i + 1, flag[2:])
            makemake_flags[i] = flag[0:2]

    handler = FlagsHandler(makefile, find_opp_configfile())
    handler.process(makemake_flags)
    return handler.target()


def extract_makemake_command(makefile_path):
    with open(makefile_path, "r", encoding="utf-8") as makefile:
        command_pattern = re.compile("#\s+(opp_makemake .*)$")
        for line in makefile:
            line_match = command_pattern.match(line)
            if line_match:
                return line_match.group(1)
            elif not line.startswith("#"):
                break

    raise ValueError("Can not find opp_makemake command call in Makefile")


def generate_cmake_target(cmake, destination):
    fh = open(destination, "w", encoding="utf-8")
    lines = ["# Generated by opp_cmake"]
    lines.append(cmake.target)
    lines.extend(cmake.target_properties)
    fh.writelines(line + '\n' for line in lines)


def main():
    if len(sys.argv) <= 2:
        print(os.path.basename(sys.argv[0]), ": <OMNeT++ Makefile> <CMake targets destination>")
    else:
        makefile = sys.argv[1]
        cmake = sys.argv[2]
        target = parse_opp_makefile(makefile)
        if target:
            generate_cmake_target(target, cmake)
        else:
            print("No targets found in ", makefile)


if __name__ == "__main__":
    main()
