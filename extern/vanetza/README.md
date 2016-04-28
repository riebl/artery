# Vanetza

Vanetza is an open-source implementation of the ETSI ITS-G5 protocol stack. It covers GeoNetworking
(GN) and Basic Transport (BTP) protocols as well as Decentralized Congestion Control (DCC) and
aspects of the Facilities layer.

# How to build

Building Vanetza is accomplished by the CMake build system. Hence, CMake needs to be available on
the build host.

## Prerequisites

You need following tools and libraries on your system for compilation of Vanetza:

* C++11 compatible compiler, e.g. GNU GCC or Clang
* CMake 3.0 or higher
* Boost 1.58 or higher
* GeographicLib 1.37 or higher

## Compilation

Following command line snippet demonstrates the build process using a generated Makefile.
Other CMake generators and build directory can be used as well.

    cd vanetza
    mkdir build && cd build
    cmake ..
    make

# Authors

Development of Vanetza is part of ongoing research work at Technische Hochschule Ingolstadt.
Maintenance is coordinated by Raphael Riebl. Contributions are happily accepted.

# License

Vanetza is licensed under LGPLv3, see LICENSE.md for details.
