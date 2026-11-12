import logging

import asyncio as aio

from pathlib import Path
from enum import IntEnum, auto
from abc import ABC, abstractmethod
from sys import stderr, stdin, stdout
from typing import Union, IO, override, Optional


logger = logging.getLogger()


class Descriptor(IntEnum):
    STDOUT = auto()
    STDERR = auto()
    STDIN = auto()


RedirectorDescriptorType = Union[int, IO]


class Redirector(ABC):
    """ Provides abstraction to redirect base file descriptors to custom destinations. """

    def __init__(self, redirects_from: Descriptor):
        self.redirects_from = redirects_from

    @abstractmethod
    async def descriptor(self) -> RedirectorDescriptorType:
        """
        Execute setup code and yield value for respective file descriptor.

        Returns:
            Tuple[int, IO]: See subprocess.popen() for accepted values.
        """
        raise NotImplementedError
    
    async def teardown(self):
        """ Clear any resources used by redirector. This will be called when subprocess is removed from executor. """
        pass
    

class NullRedirector(Redirector):
    """ Disable passing standard streams entirely. """

    def __init__(self, redirects_from: Descriptor):
        super().__init__(redirects_from)

    @override
    async def descriptor(self) -> RedirectorDescriptorType:
        return aio.subprocess.DEVNULL


class PassthroughRedirector(Redirector):
    """ Pass through to current STDOUT, STDIN or STDERR directly. Useful for wrapper scripts. """

    def __init__(self, redirects_from: Descriptor, redirect_to: Optional[Descriptor] = None):
        super().__init__(redirects_from)
        self.redirects_to = redirect_to

    @override
    async def descriptor(self) -> RedirectorDescriptorType:
        redirect_to = self.redirects_from
        if self.redirects_to is not None:
            redirect_to = self.redirects_to

        match redirect_to:
            case Descriptor.STDERR:
                return stderr
            case Descriptor.STDOUT:
                return stdout
            case Descriptor.STDIN:
                return stdin
            case _:
                raise ValueError


class FileRedirector(Redirector):
    """ Saves output to a file or takes input from a file. """

    def __init__(self, redirects_from: Descriptor, file: Path):
        super().__init__(redirects_from)
        self._fd: Optional[IO] = None
        self.file = file

    def _open_for_read(self, file: Path) -> IO:
        self._fd = file.open('r')
        return self._fd
    
    def _open_for_write(self, file: Path) -> IO:
        self._fd = file.open('w+')
        return self._fd

    @override
    async def descriptor(self) -> RedirectorDescriptorType:
        try:
            if self.redirects_from == Descriptor.STDIN:
                return self._open_for_read(self.file)
            else:
                return self._open_for_write(self.file)
        except PermissionError as err:
            logger.error('failed to open file for standard descriptor redirection, %s', self.file, exc_info=err)
            raise

    async def teardown(self):
        self._fd.close()
