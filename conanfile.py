import os

from conan import ConanFile
from conan.tools.cmake import cmake_layout

class Artery(ConanFile):
    version = "0.0"
    generators = ["CMakeToolchain", "CMakeDeps"]
    settings = "os", "compiler", "build_type", "arch"
    tool_requires = ["protobuf/3.21.9"]

    def requirements(self):
        for req in [
            'boost/1.86.0',
            'cryptopp/8.9.0',
            'protobuf/3.21.9',
            'geographiclib/2.3',
            'cppzmq/4.10.0',
            'plog/1.1.10'
        ]:
            self.requires(req)

    def layout(self):
        build_dir = 'build'
        if os.environ.get('CONTAINER_BUILD_DIR') is not None:
            build_dir = 'container_build'
        cmake_layout(self, build_folder=build_dir)
