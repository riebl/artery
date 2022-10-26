# Work with VS Code

Visual Studio Code (VS Code) is a popular code editor which can be customized by extensions (plug-ins).
Since Artery is mostly a plain C++ project using CMake as its build tool, one can use VS Code instead of the Eclipse-based OMNeT++ IDE for development.

## Requirements
Install [VS Code](https://code.visualstudio.com/docs/setup/setup-overview) along with its [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) and [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) Extensions.

## Building and Running Simulations
First, open Artery's folder with VS Code. Press `Ctrl+Shfit+P` to execute the following commands:
1. `CMake: Scan for Kits`
2. `CMake: Select a Kit` to select a compiler kit (GCC/Clang)
3. `CMake: Set Build Target` and choose your scenario (in doubt: `run_example`)
4. `CMake: Build` to start the simulation

## Debugging Simulations
1. Add `launch.json` to `.vscode/`. Adjust paths and arguments to your system/needs:
```
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "clang++ - Build and debug active file",
            "type": "cppdbg",
            "request": "launch",

            // Set path to OMNet++ debug runner
            "program": "<path>/omnetpp-5.6.2/bin/opp_run_dbg",

            // Set working directory to scenario
            "cwd": "${workspaceFolder}/scenarios/artery",

            "environment": [
                { "name": "ARTERY_HOME", "value": "<path>" },
                { "name": "OMNETPP_HOME", "value": "<path>" },
                { "name": "OMNETPP_ROOT", "value": "<path>" },
                { "name": "SUMO_HOME", "value": "<path>" }
            ],
            "args": [
                // Paths to NED files
                "-n${ARTERY_HOME}/src/artery:${ARTERY_HOME}/src/traci:${ARTERY_HOME}/extern/veins/examples/veins:${ARTERY_HOME}/extern/veins/src/veins:${ARTERY_HOME}/extern/inet/src:${ARTERY_HOME}/extern/inet/examples:${ARTERY_HOME}/extern/inet/tutorials:${ARTERY_HOME}/extern/inet/showcases",

                // Libraries used for Artery
                "-l${ARTERY_HOME}/build/src/artery/envmod/libartery_envmod.so",
                "-l${ARTERY_HOME}/build/extern/libINET.so",
                "-l${ARTERY_HOME}/build/extern/libveins.so",
                "-l${ARTERY_HOME}/build/src/artery/libartery_core.so",
                "-l${ARTERY_HOME}/build/src/traci/libtraci.so",
                // "-l${ARTERY_HOME}/build/scenarios/highway-police/libartery_police.so",

                // .ini file to of the scenario
                "omnetpp.ini",

                // OMNet++ config to run
                "-cenvmod",
                // Run number
                "-r0",

                // OMNet++ interface can either be Qtenv or Cmdenv
                //"-uQtenv",
                "-uCmdenv",
            ],
            "stopAtEntry": false,
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "Enable all-exceptions",
                    "text": "catch throw",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: clang++ build active file",
            "miDebuggerPath": "/usr/bin/gdb"
        }
    ]
}
```
2. Add `tasks.json` to `.vscode/`
```
{
    "tasks": [
        {
            "type": "cppbuild",
            "label": "C/C++: clang++ build active file",
            "command": "/usr/bin/cmake",
            "args": [
                "--build",
                ".",
                "--parallel 6"
            ],
            "options": {
                "cwd": "${workspaceFolder}/build"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "detail": "Generated task by Debugger"
        }
    ],
    "version": "2.0.0"
}
```
3. Execute `CMake: Select Variant` and select `Debug`
4. In case of doubt: execute `CMake: Clean Rebuild`
5. Switch to debug view with `Ctrl+Shift+D`
6. Start debugging by pressing `F5`

!!! note "Credits"
    Thanks to [Alexander Willecke](https://github.com/awillecke) for contributing this page.
