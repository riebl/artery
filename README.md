# Artery

Artery enables V2X simulations based on ETSI ITS-G5 protocols like GeoNetworking and BTP. Single vehicles can be equipped with multiple ITS-G5 services through Artery's middleware, which also provides common Facilities for these services.

Artery is an extension to the Veins framework. Documentation of Veins is available [on its website](http://veins.car2x.org).

## Requirements

You need a C++ Compiler with C++11 support, Boost and Vanetza libraries for building Artery.
We have tested Artery with OMNeT++ 4.6, GNU GCC 4.9 and Boost 1.56 successfully.
You can obtain a recent version of Vanetza from [the github repository](https://github.com/riebl/vanetza).

## Build instructions

Artery requires Vanetza libraries for the ITS-G5 protocols. Create a file *local.configure* in the Artery root directory (where this README file is located) with path information to your local Vanetza installation. In the following example, Vanetza is located at */path/to/your/vanetza* and you have built it there in a subdirectory called *build*:

    [vanetza]
    root=/path/to/your/vanetza
    lib_dir=%(root)s/build/lib

You might want to add a further section to the *local.configure* file if you have built Vanetza with a custom version of Boost, i.e. one not located in your build environment's default search path. Following example works if Boost sources are extracted and built at */path/to/your/boost*:

    [boost]
    root=/path/to/your/boost
    inc_dir=%(root)s
    lib_dir=%(root)s/stage/lib

After these configuration steps, please execute Artery's *configure* script and start a build with Vanetza integration:

    ./configure --with-vanetza
    make

When compilation has finished, you can run the provided Artery example to test if everything is built correctly:

    ./sumo-launchd.py
    cd examples/artery
    ./run
