import conan
import typing


class Artery(conan.ConanFile):
    generators: typing.List[str] = ['CMakeToolchain', 'CMakeDeps']
    settings: typing.List[str] = ['os', 'compiler', 'build_type', 'arch']

    def __init__(self: 'Artery', display_name: str = '') -> None:
        self._requirements = {
            'boost': '1.86.0',
            'cryptopp': '8.2.0',
            'geographiclib': '2.3'
        }
        super().__init__(display_name)

    def requirements(self: 'Artery') -> None:
        for req in self._requirements:
            self.requires(f'{req}/{self._get_version(req)}')

    def layout(self: 'Artery') -> None:
        conan.tools.cmake.cmake_layout(
            self,
            build_folder=self._get_build_directory(),
            src_folder=self._get_source_directory()
        )

    def _get_conf_var(self: 'Artery', var: str, default: typing.Any = None) -> typing.Any:
        return self.conf.get(var, default=default)

    def _get_version(self: 'Artery', package: str) -> str:
        return self._get_conf_var(f'user.{package}:version', self._requirements[package])

    def _get_build_directory(self: 'Artery', default: str = 'build') -> str:
        return self._get_conf_var('user.recipe:build_dir', default)

    def _get_source_directory(self: 'Artery', default: str = '.') -> str:
        return self._get_conf_var('user.recipe:source_dir', default)
