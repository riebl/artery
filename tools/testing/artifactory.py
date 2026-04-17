import pytest

from pathlib import Path
from contextlib import contextmanager
from typing import Generator, Self, Mapping, Dict

from tools.testing.executors import FileRedirector, Descriptor


class Artifactory:
    def __init__(self, artifactory_root: Path):
        self.current_root = artifactory_root

    def add_file(self, filename: Path) -> Path:
        resolved = self.current_root / filename
        if resolved.is_file():
            raise FileExistsError(f'file {resolved} already present in reports directory')
        return resolved

    @contextmanager
    def add_directory(self, directory: Path) -> Generator[Self]:
        self.current_root = self.current_root / directory
        if self.current_root.is_dir():
            raise FileExistsError(f'directory {self.current_root} already present in reports directory')

        yield self
        
        self.current_root = self.current_root.parent


@pytest.fixture
def artifactory(run_id, request) -> Artifactory:
    run_artifactory = Artifactory(request.config.getoption('--reports-dir'))
    with run_artifactory.add_directory(run_id):
        return run_artifactory


class PytestOutputFileRedirector(FileRedirector):
    """ File redirector, suited to use with local reports feature. Also, this one is verbose. """

    counters: Dict[str, int] = {}

    @classmethod
    def format_filename(cls, cmd: str, descriptor: Descriptor) -> Path:
        cls.counters[cmd] = cls.counters.get(cmd, 0) + 1
        match descriptor:
            case Descriptor.STDOUT:
                return Path(f'{cls.counters[cmd]}_stdout')
            case Descriptor.STDERR:
                return Path(f'{cls.counters[cmd]}_stderr')
            case _:
                raise ValueError

    @classmethod
    def output_descriptors(cls, cmd: str, artifactory: Artifactory) -> Mapping[Descriptor, FileRedirector]:
        """
        Returns file descriptors for both stderr and stdout, placed within command's own directory. 

        Args:
            cmd (str): command that owns descriptors.
            artifactory (Artifactory): artifactory instance for current run.

        Returns:
            Mapping[Descriptor, FileRedirector]: mapping with redirectors created.
        """

        with artifactory.add_directory(Path(cmd)):
            path_to_stdout = artifactory.add_file(cls.format_filename(cmd, Descriptor.STDOUT))
            path_to_stderr = artifactory.add_file(cls.format_filename(cmd, Descriptor.STDERR))

        stdout_redirector = FileRedirector(Descriptor.STDOUT, path_to_stdout)
        stderr_redirector = FileRedirector(Descriptor.STDERR, path_to_stderr)

        return {
            Descriptor.STDOUT: stdout_redirector,
            Descriptor.STDERR: stderr_redirector,
        }
