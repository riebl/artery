# Devcontainer support

A [development container](https://containers.dev) (or devcontainer for short) allows you to use a container as a development environment. It can be used to develop and run applications in the isolated environment that is persistent across different hosts. This page describes the configuration for VS Code. You can refer to [this](https://containers.dev/supporting) page for other supported tools.

## Requirements

- Install [VS Code](https://code.visualstudio.com/docs/setup/setup-overview)
- Install [devcontainers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

## Develop inside the container

Working with devcontainers is very similar to [remote development](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-ssh) using ssh. 
After installing devcontainer extension, you should observe
an icon in status bar of your editor that looks like two arrowheads pointing at each other. Clicking on that
icon reveals a menu for remote connections, which should include an option for opening current directory
inside a container:

`Reopen in Container`

Selecting this entry brings you a list of options for containers. The ones Artery provides are called _arch-linux_ and _debian_.
You can choose any of them, but keep in mind that graphic applications (including Artery) might fail due to incompatible versions between
host and the container.

Same can be achieved with Command palette:

- Press <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>P</kbd>
- Select `Reopen in Container`

After choosing that, a VS Code window should reappear with connection established. If it's your first
time opening, container will require to be built first, you can click on `view logs` in notification on the right
side to track progress.

## Build.py and Conan

### Build.py

Although this is not part of the topic, Artery has some handy tools are quite useful inside the devcontainer. `build.py` script
helps you running CMake. The idea behind it is to alter CMake's behavior with as little effort as possible
by just adding or removing short options for the script.

Let's look more closely.

The script consists of various _routines_ that are essentially just different shell commands that
are being run under a hood:

1. remove (-r) - clears build directories for configurations, for example build/Debug, build/Release
2. conan-install (-i) - runs conan installation command, generates presets to use use with CMake
3. configure (-c) - runs CMake configuration step
4. build (-b) - runs CMake build step
5. symlink-compile-commands (-l) - generates a symlink to compile_commands.json in one of build/<config> directories

