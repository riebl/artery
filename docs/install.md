# Install Instructions

We guide you through the process of preparing, building and running Artery with the following steps.
First of all, you will need to [clone the Artery](#cloning-the-artery-repository) repository on your machine.
Afterwards, you can either [manually build](#building-artery-from-sources) Artery for your Linux machine or create a Linux-based [virtual machine](#automated-creation-of-a-virtual-machine) using
[Vagrant](https://vagrantup.com).

## Cloning the Artery repository

All source code of Artery is stored in a [git](https://git-scm.com) repository on [GitHub](https://github.com/riebl/artery).
This repository contains compatible versions of INET, SimuLTE, Veins, Vanetza and some other third-party dependencies as
[git submodules](https://git-scm.com/docs/git-submodule).
You can find those submodules in the *extern* subdirectory.

Please make sure to fetch these submodules when cloning the Artery repository.
The command below will clone Artery's _master_ branch along with matching versions of its submodules onto your machine's
file system.
Your local clone of the repository will be in the *artery* directory at the location you have invoked the clone command.
We refer to the location of your local clone as **$ARTERY_PATH** in the following.

    :::shell
    git clone --recurse-submodule https://github.com/riebl/artery.git

Alternatively, you can also initialise and fetch these submodules after cloning.
This command needs to be invoked from **$ARTERY_PATH**.

    :::shell
    git submodule update --init --recursive

!!! tip
    Check if the directories in *$ARTERY_PATH/extern* are empty.
    If they are empty, e.g. no *extern/inet/Makefile* exists, then your working copy is incomplete.
    Fix your working copy first because the build process will fail otherwise.


## Building Artery from sources

Following instructions show you how to build Artery from its sources on a Linux machine step by step.
While it is probably not impossible to build Artery on a non-Linux system, this is neither supported nor tested at all.
Instead, consider to set up a virtual machine for Artery if you do not have a Linux system at your fingertips.
The [alternative build instructions](#automated-creation-of-a-virtual-machine) at the end of this page show you how to build a virtual machine with Artery installed inside automatically.
But let us now continue with the requirements for the manual build process.

### Requirements

Artery builds upon the discrete event simulator [OMNeT++](https://omnetpp.org), which you need to obtain and install beforehand.
Instead of *opp_makemake* usually employed by OMNeT++ models, Artery's build process relies on [CMake](https://cmake.org).
This enables us to handle external dependencies beyond OMNeT++ conveniently.
Hence, make sure the following requirements are fulfilled:

- C++ compiler with C++11 support (GNU GCC 4.9 or later or Clang)
- [Boost](https://www.boost.org) (1.65.1 or later is recommended)
- [CMake](http://www.cmake.org) 3.1 or later
- [OMNeT++](https://omnetpp.org) 5.4 or later
- [Python](https://www.python.org) 3

We have tested Artery with OMNeT++ 5.4, GNU GCC 7.3 and Boost 1.65.1 successfully.

Vanetza is an integral part of Artery because it provides the ITS-G5 network stack.
On top of the dependencies listed above, Vanetza requires

- [GeographicLib](https://geographiclib.sourceforge.io) 1.37 or later
- [Crypto++](https://www.cryptopp.com) 5.6 or later

You may install the packages `libgeographic-dev` and `libcrypto++-dev` on a Debian system for these.
The packages `libboost-dev`, `libboost-date-time-dev`, and `libboost-system-dev` provide the required Boost heades and libraries.

Though not required for compiling Artery, you will also need to install [SUMO](https://sumo.dlr.de).
Ideally, the *sumo* executable is located in your **$PATH**.
Then, Artery can launch a *sumo* instance for you when starting a simulation.
You will need version 1.0 or later of SUMO because its TraCI protocol of earlier versions is not compatible.


### Build 'extern' components

INET, Vanetza, and Veins located in their respective *extern* subdirectories need to be built before Artery itself.
A *Makefile* is located in Artery's root directory to make this build step more convenient.
Simply call `make all` from this directory and INET, Veins and Vanetza will be built accordingly.
Optionally, if you intend to enable the SimuLTE integration, call `make simulte` in addition.

#### Vanetza
Our *Makefile* tries to build Vanetza in *extern/vanetza/build*.
Usually, if all dependencies are installed system-wide, no manual intervention is necessary.
Since Vanetza is using CMake as well, you can customise its build directory with the usual CMake tools.
Please consult the [Vanetza website](https://www.vanetza.org) and its [Readme](https://github.com/riebl/vanetza/blob/master/README.md) for details.

#### INET Framework
The `make inet` target resembles the install steps given in the [INET install guide](https://github.com/inet-framework/inet/blob/master/INSTALL).
As long as OMNeT++ is installed in your environment, this should run smoothly.

#### Veins
Please make sure you have all dependencies of Veins installed and set up OMNeT++ beforehand.
Veins' build scripts depend on Python, so a Python interpreter needs to be accessible on your system.
If you have any doubts, refer to the [Veins tutorial](http://veins.car2x.org/tutorial).


### Create build directory
Are you still with us? Congratulations, you are almost done!
Invoke following commands from **$ARTERY_PATH** to create a *build* directory for Artery, configure the build directory with CMake and finally build Artery there.

    :::shell
    mkdir build
    cd build
    cmake ..
    cmake --build .

!!! note
    Artery's build directory does not be located at *$ARTERY_PATH/build*.
    You can name it in any way you like and also have multiple build directories, e.g. one for release and one for debug builds.
    Out-of-source builds are supported as well.


### Automated creation of a virtual machine

You can build a virtual machine containing Artery from scratch assisted by Vagrant and our Ansible deployment scripts.
For this procedure, you have to install [Vagrant](https://www.vagrantup.com) and [VirtualBox](https://www.virtualbox.org) on your host machine.
Your host system may use any operating system supported by Vagrant and VirtualBox, including MacOSX and Windows.

Your working copy of Artery at **$ARTERY_PATH** contains a *Vagrantfile*.
This file tells Vagrant to download a Debian machine image and deploy Artery in this virtual machine according to the
Ansible playbook located at *ansible/vagrant.yml*.
Just run `vagrant up` from Artery's root directory, be patient, and in the end you are rewarded with a ready-to-go
environment to work with Artery.
Login credentials for this virtual machine, as by convention for any Vagrant box, are "vagrant" with password "vagrant".
Of course, you are free to change the password at your discretion.

During the initial provisioning of this virtual machine, when the Ansible deployment is running, an Internet connection is required.
Without an Internet connection, it is impossible to download all the dependencies such as OMNeT++, SUMO, and several system tools and libraries.
In case of an interrupted deployment, you may restart the provisioning with `vagrant up --provision`.
Also keep in mind that this initial deployment can take quite some time.
Afterwards, the virtual machine can be booted much faster.

You can run the example from the *artery-build* directory located in the box's home directory at */home/vagrant/* by
invoking `make run_example`.
Please note that the *artery* directory mirrors the cloned repository on your host machine.

![Starting Artery in virtual machine supplied by Vagrant](../assets/vagrant.webm){:autoplay loop}

## Running Artery for the first time

When you have finished building Artery, you can give the shipped example a try to see if everything runs smoothly.
With current Artery versions, there is no need to start Veins' *sumo-launchd* script any more.
Instead, Artery will start a SUMO instance on its own with appropriate parameters.
You can start the example scenario of Artery (located in *scenarios/artery*) simply by invoking the **run_example** target from Artery's root directory:

    :::shell
    cmake --build build --target run_example

!!! note
    Substitute *build* with the name of your build directory.
    If CMake has generated Makefiles in your build directory (it does so by default), you can also invoke `make run_example` from there.

Please make sure that *sumo* can be launched in your environment because this is the SUMO executable Artery uses by default.
You can, however, specify which SUMO executable shall be used explicitly.
If you want Artery to start SUMO with a graphical user interface, you can put the following line in *omnetpp.ini*:

    *.traci.launcher.sumo = "sumo-gui"
