# Artery

Artery enables V2X simulations based on ETSI ITS-G5 protocols like GeoNetworking and BTP.
Single vehicles can be equipped with multiple ITS-G5 services through Artery's middleware, which also provides common Facilities for these services.

Artery is an extension to the [Veins framework](http://veins.car2x.org).
Please refer to its [documentation](http://veins.car2x.org/documentation) for details about Veins.

## Requirements
You need a C++ Compiler with C++11 support, Boost and Vanetza libraries for building Artery along with Veins.
Artery and Veins build upon the discrete event simulator [OMNeT++](https://omnetpp.org), which you need to obtain as well.
We have tested Artery with OMNeT++ 5.1, GNU GCC 6.3 and Boost 1.62 successfully.
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
You can build all of these external project dependencies at once via the default `make all` target.

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

### INET
As the next step, you need to build INET. Make sure you are in the root directory of Artery and simply call `make inet` there.
INET's build dependencies are listed in the INET [Install manual](extern/inet/INSTALL).

### Artery
Are you still with us? Congratulations, you are almost done!

    mkdir build
    cd build
    cmake ..
    cmake --build .

These steps create a *build* directory for Artery, configure the build directory and finally build Artery there.

## Running Artery

When you have finished building Artery, you can give the shipped example a try to see if everything runs smoothly.
With current Artery versions there is no need to start Veins' *sumo-launchd* script any more.
Instead, Artery will start a SUMO instance on its own with appropriate parameters.
You can start the example scenario of Artery (located in *scenarios/artery*) simply by calling the **run_example** target from Artery's root directory:

    cmake --build build --target run_example

Please make sure that *sumo* can be executed within in your environment because this is the default SUMO executable used by Artery.
You can, however, specify which SUMO executable shall be used explicilty.
If you want Artery to start SUMO with a graphical user interface, you can put the following line in your *omnetpp.ini*:

    *.traci.launcher.sumo = "sumo-gui"
