# Devcontainer support

A [development container](https://containers.dev) (or devcontainer for short) allows you to use a container as a development environment. It can be used to develop and run applications in the isolated environment that is persistent across different developers. This page describes the configuration for VS Code. You can refer to [this](https://containers.dev/supporting) page for other supported tools.

## Requirements

- Install [VS Code](https://code.visualstudio.com/docs/setup/setup-overview)
- Install [Devcontainers Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)
- Open the root directory of your cloned Artery repository in VSCode

## Develop inside the container

Working with devcontainers is very similar to [remote development](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-ssh) using __ssh__.  After installing devcontainer extension, you should observe
an icon in the status bar of your editor that looks like two arrowheads pointing at each other. Clicking on that
icon reveals a menu for remote connections, which should include an option for opening current directory
inside a container:

`Reopen in Container`

Selecting this entry brings you a list of options for containers. The ones Artery provides are called _arch-linux_ and _debian_,
built upon ArchLinux and Debian image respectively.

!!! danger
    Keep in mind that sharing X11 displays between your host and the container environment may fail and require tweaking on your specific system.

Same can be achieved with _Command palette_:

- Press <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>P</kbd>
- Select `Dev Container: Reopen in Container`

After choosing that, a VS Code window should reappear with connection established. If it's your first
time opening, container will require to be built first. You can click on `view logs` in notification on the right
side to track progress.

The opened Artery directory will be mounted in the container and changes that you make to the source code 
or other files in that directory are saved in your host's filesystem.

!!! warning
    Please use distinct build directories for host and devcontainer builds if you want to 
    build Artery in both environments.

## Building inside the container

This section presents some handy tools that you might want to use inside the devcontainer.

### `build.py`

`build.py` is one of them - it helps you running CMake. The idea behind it is to execute common tasks 
with as little effort as possible by just adding or removing short options for the script.

Let's look at how it operates more closely.

The script consists of various _routines_ that are essentially just different shell commands that
are being run under a hood:

| Command | Short option in CLI | Description |
|---------|---------------------|-------------|
| remove | -r |  clears directories for specific build configurations, for example `build/Debug` or `build/Release` |
| conan-install | -i |  runs Conan installation command and generates presets to use use with CMake | 
| configure | -c | runs CMake configuration step |
| build | -b | runs CMake build step |
| symlink-compile-commands | -l | generates a symlink to compile_commands.json in one of `build/<config>` directories |

Here, routines are provided in order of their execution. Apart from them, you can also pass down to shell
some configuration options:

- `build-dir` - sets root build directory, defaults to `build`
- `config` - build configurations for CMake, can be specified multiple times. (_Debug_/_Release_, defaults to _Debug_ only)
- `parallel` - number of threads to use during build, defaults to number of your processor's cores
- `generator` - generator for CMake, defaults to _Ninja_
- `profile` - profile for Conan, you can pass a name or a path, defaults to `default`

Usually these options impact only one routine but `config` and `build-dir` are special. You can
think of them as some sort of 'scope' for the script: for example, if you specify only _Release_
build config and set `build-dir` to `some_other_build`, then all routines consider only `some_other_build/Release`
(remove removes only `some_other_build/Release`, build builds `some_other_build/Release` etc.)

Some examples:

Run CMake configure & build, dependencies are considered system-wide:
```shell
./build.py -cb
```

Same as above, but use Unix Makefiles as generator and build using 16 cores:
```shell
./build.py -cb --parallel 16 --generator "Unix Makefiles"
```

Run CMake configure & build run for both Debug and Release, clear `build/Debug` and `build/Release` first:
```shell
./build.py -rcb --config Release --config Debug
```

Same as above, but generate a symlink after (to `build/Debug/compile_commands.json` if _Debug_ is present, 
`build/Release/compile_commands.json` otherwise):
```shell
./build.py -rcbl --config Release --config Debug
```

Same as above, but also run `conan-install` with `default` profile:
```shell
./build.py -ricbl --config Release --config Debug
```

### Conan

!!! note
    Using conan is not necessary: containers provide all dependencies anyway. 

[Conan](https://conan.io) is one of the package providers for C++ development, operating independently from project's build system.
Conan needs two things to compile and provide packages for you:

1. profile
2. recipe

Profile is a description for your system. Typically it looks like this:

```ini
[settings]
arch=x86_64
build_type=Release
compiler=gcc
compiler.cppstd=gnu14
compiler.libcxx=libstdc++11
compiler.version=14
os=Linux
```

You can let Conan guess these settings with a simple command: `conan profile detect`. Afterwards, you
may want to edit and adjust that generated file. Profiles are system-wide, which means that you can
use one profile in a couple of projects simultaneously.

Recipe defines configuration for a certain project. Older versions of Conan used the same .ini files
for recipes, but with newer releases you can provide python file instead. Most of the time you should just
specify packages and versions that you want, coupled with default settings for CMake:

```python  linenums="1"
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
```

With recipe and profile, Conan installs packages in global index with settings provided by profile,
generating a CMake toolchain file which may be included with CMake's preset option:

```
cmake --preset conan-debug
```

`build.py` handles that for you.

Artery provides profiles for containers under `conan/` and recipe under root directory: `conanfile.py`.
Inside the devcontainer you just have to specify routine (-i, `conan-install`) and path to one of those profiles
to invoke Conan:

```shell
./build.py -icb --config Release --profile $PWD/conan/debian-x86-64.ini
```
