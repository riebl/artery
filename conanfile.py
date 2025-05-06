import conan

from pathlib import Path

from conan.tools.scm import Git
from conan.tools.files import copy
from conan.errors import ConanException
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout


class Artery(conan.ConanFile):
    name = 'artery'
    settings = ['os', 'compiler', 'build_type', 'arch']
    homepage = 'https://github.com/riebl/artery'
    license = 'GNU-2.0'

    options = {
        'sources': ['git/master'],
        'fpic': [True, False],
        'with_simulte': [True, False],
        'with_envmod': [True, False],
        'with_storyboard': [True, False],
        'with_trunsfusion': [True, False],
        'with_testbed': [True, False],
        'with_ots': [True, False],
        'vscode_launch_integration': [True, False]
    }

    default_options = {
        'sources': 'git/master',
        'fpic': True,
        'with_simulte': True,
        'with_envmod': True,
        'with_storyboard': True,
        'with_trunsfusion': False,
        'with_testbed': False,
        'with_ots': False,
        'vscode_launch_integration': False
    }

    def requirements(self):
        for req, version in self.conan_data['requirements'].items():
            self.requires(f'{req}/{version}')

    def layout(self):
        cmake_layout(self)

    def validate(self):
        check_min_cppstd(self, '17')

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, 'COPYING', src=self.source_folder, dst=Path(self.package_folder) / 'licenses')
        cmake = CMake(self)
        cmake.install()

    def set_version(self):
        git = Git(self, folder=self.recipe_folder)
        self.version = git.get_commit()[:8]
        if not git.is_dirty():
            self.version += '-dirty'

    def generate(self):
        tc = CMakeToolchain(self)

        if self.options['fpic']:
            tc.variables['CMAKE_POSITION_INDEPENDENT_CODE'] = True
        
        for cmake_boolean_option in (
            'with_simulte',
            'with_envmod',
            'with_storyboard',
            'with_trunsfusion',
            'with_testbed',
            'with_ots',
            'vscode_launch_integration'
        ):
            tc.cache_variables[cmake_boolean_option.upper()] = \
                getattr(self.options, cmake_boolean_option)

        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def source(self):
        setting = self.conan_data['fetch_sources'].split('/')
        if len(setting) != 2:
            raise ConanException(f'expected sources in the following format: "method/presets_name", got {setting}')
        method, presets_name = setting

        if method not in self.conan_data['sources']:
            methods = self.conan_data['sources'].keys()
            raise ConanException(f'no such method {method}, methods provided: ' + ', '.join(methods))
        
        method_presets = self.conan_data['sources'][method]
        if presets_name not in method_presets:
            available_presets = method_presets.keys()
            raise ConanException(
                f'no such preset {presets_name}, presets provided for method {method}: ' + ', '.join(available_presets)
            )
        
        args = method_presets[presets_name]
        match method:
            case 'git':
                git = Git(self)
                git.clone(**args, target='.', hide_url=False)
            case _:
                raise ConanException(f'method {method} is not implemented by this conanfile')
