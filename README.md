# Artery

Artery enables V2X simulations based on ETSI ITS-G5 protocols like GeoNetworking and BTP.
Single vehicles can be equipped with multiple ITS-G5 services through Artery's middleware, which also provides common Facilities for these services.

Artery is an extension to the [Veins framework](http://veins.car2x.org).
Please refer to its [documentation](http://veins.car2x.org/documentation) for details about Veins.

## Requirements
You need a C++ Compiler with C++11 support, Boost and Vanetza libraries for building Artery along with Veins.
Artery and Veins build upon the discrete event simulator [OMNeT++](https://omnetpp.org), which you need to obtain as well.
We have tested Artery with OMNeT++ 4.6, GNU GCC 5.3 and Boost 1.58 successfully.
Only [CMake](http://www.cmake.org) is the officially supported way for building Artery.

Compatible versions of Veins and Vanetza are part of the Artery repository, see the *extern* subdirectory.
These dependencies are managed with the help of [git-subrepo](https://github.com/ingydotnet/git-subrepo), i.e. you can just clone our Artery repository and get appropriate Veins and Vanetza sources automatically.
You might obtain more recent versions from their upstream repositories:

- [Veins](https://github.com/sommer/veins)
- [Vanetza](https://github.com/riebl/vanetza)


## Build instructions
Veins and Vanetza need to be built before Artery itself.
For this purpose, a *Makefile* is located in Artery's root directory.
The following steps assume you have cloned the Artery repository on your (preferably Linux) machine and you have moved your terminal to this clone's directory.

### Veins
Please make sure you have all dependencies of Veins installed and set up OMNeT++ beforehand.
If you have any doubts, refer to the [Veins tutorial](http://veins.car2x.org/tutorial).
The bundled version of Veins is then built with following steps:

    cd extern/veins
    ./configure
    make

Alternatively, you can also just invoke `make veins` in the root directory of Artery, which executes exactly these steps but saves you some typing.

### Vanetza
Similarly, you can simply call `make vanetza` in the root directory and it will try to build Vanetza for you in *extern/vanetza/build*.
Of course, you need to install Vanetza's dependencies first.
See the Vanetza [Readme](extern/vanetza/README.md) for details.

### Artery
Are you still with us? Congratulations, you are almost done!

    mkdir build
    cd build
    cmake ..
    cmake --build .

These steps create a *build* directory for Artery, configure the build directory and finally build Artery there.

## Running Artery

When you have finished building Artery, you can give the shipped example a try to see if everything runs smoothly.
First, start Veins' *sumo-launchd* script.
You can do this either manually or simply use the integrated **launch_sumo** target, e.g. when you have used CMake's default Makefile generator:

    cd build
    make launch_sumo

You are only required to start the SUMO launcher once because it keeps running in the background.
The **run_example** target starts the Artery example scenario located in *scenarios/artery*.
