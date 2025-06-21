import conan

from typing import Callable, Mapping, Any
from conan.tools.cmake import cmake_layout


class Artery(conan.ConanFile):
    name = 'artery'
    generators = ['CMakeToolchain', 'CMakeDeps']
    settings = ['os', 'compiler', 'build_type', 'arch']

    # dynamically set conanfile attributes
    conan_data: Mapping[str, Any]
    requires: Callable[[str], None]

    def requirements(self):
        for req, version in self.conan_data['requirements'].items():
            self.requires(f'{req}/{version}')

    def layout(self):
        cmake_layout(self)
