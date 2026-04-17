import logging

import asyncio as aio

from abc import ABC
from typing import Optional, Mapping, Dict, Any, Tuple, cast

from .redirectors import Redirector, Descriptor, NullRedirector


logger = logging.getLogger(__name__)


SubprocessMeta = Tuple[aio.subprocess.Process, int]


class AsyncExecutor(ABC):
    """
    Async executor for certain program. Users are supposed to add methods, reflecting binary functionality.
    Look into ancestors of this class for examples.
    """

    def __init__(self, cmd: str):
        self.cmd = cmd
        self.process: Optional[aio.subprocess.Process] = None

        self.redirectors: Dict[Descriptor, Redirector] = {
            Descriptor.STDIN: NullRedirector(Descriptor.STDIN),
            Descriptor.STDOUT: NullRedirector(Descriptor.STDOUT),
            Descriptor.STDERR: NullRedirector(Descriptor.STDERR),
        }

    @property
    def _current_process(self) -> aio.subprocess.Process:
        if self.process is None:
            raise ValueError('this executor currently does not own any process')
        return self.process

    async def _start(self, redirectors: Mapping[Descriptor, Redirector], *options: str, **kwargs: Any):
        self.redirectors.update(redirectors)

        stdin, stdout, stderr = await aio.gather(
            *(self.redirectors[descriptor].descriptor() for descriptor in (Descriptor.STDIN, Descriptor.STDOUT, Descriptor.STDERR))
        )

        logger.info('starting up command %s with args: %s', self.cmd, ' '.join(options))

        self.process = await aio.subprocess.create_subprocess_exec(
            self.cmd,
            *options,
            stdin=stdin,
            stdout=stdout,
            stderr=stderr,
            **kwargs,
        )
    
    async def _wait(self, timeout: Optional[float]) -> Optional[SubprocessMeta]:
        try:
            logger.debug('awaiting %s to finish with %d seconds', self.cmd, timeout)
            code = await aio.wait_for(self._current_process.wait(), timeout)

            logger.debug('process for command %s finished, exit code %d', self.cmd, code)
            return await self._cleanup(), code

        except TimeoutError:
            logger.warning('reached timeout %d for command %s: cannot wait any longer', timeout, self.cmd)
            return None
        
    async def _cleanup(self) -> aio.subprocess.Process:
        process, self._process = self._current_process, None

        async with aio.TaskGroup() as grp:
            for redirector in self.redirectors.values():
                grp.create_task(redirector.teardown())
            
        self.redirectors = {descriptor: NullRedirector(descriptor) for descriptor in self.redirectors}
        return process

    async def wait(self, timeout: Optional[float] = 5.0) -> Optional[SubprocessMeta]:
        return await self._wait(timeout)
    
    async def wait_and_kill(self, timeout: Optional[float] = 5.0) -> SubprocessMeta:
        retval = await self._wait(timeout)
        if retval is not None:
            return retval
        
        self._current_process.kill()

        # we can cast safely, since we know that wait() is run with disabled timeout
        return cast(SubprocessMeta, await self._wait(None))
