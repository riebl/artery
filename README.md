# Artery

Artery enables V2X simulations based on ETSI ITS-G5 protocols like GeoNetworking and BTP.
Single vehicles can be equipped with multiple ITS-G5 services through Artery's middleware, which also provides common Facilities for these services.

Artery started as an extension of the [Veins framework](http://veins.car2x.org) but can be used independently nowadays.
Please refer to its [documentation](http://veins.car2x.org/documentation) for details about Veins.

## Requirements
You need a C++ Compiler with C++11 support, Boost and Vanetza libraries for building Artery along with Veins.
Artery and Veins build upon the discrete event simulator [OMNeT++](https://omnetpp.org), which you need to obtain as well.
We have tested Artery with OMNeT++ 5.2, GNU GCC 7.3 and Boost 1.65.1 successfully.
Only [CMake](http://www.cmake.org) is the officially supported way for building Artery.

Compatible versions of INET, Veins, Vanetza, and other components are managed as [git submodules](https://git-scm.com/docs/git-submodule) in the *extern* subdirectory.
Please make sure to fetch these submodules when cloning our repository!

    git clone --recurse-submodule https://github.com/vandit86/artery.git

Alternatively, you can also load these submodules after cloning:

    git submodule update --init --recursive

You might obtain more recent versions from their upstream repositories:

- [INET](https://github.com/inet-framework/inet)
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
INET's build dependencies are listed in [its install manual](extern/inet/INSTALL).

### LTE 
// Vadym add.. 
Need to build LTE to use LTE capability on simulations 
`make simulte`


### Artery
Are you still with us? Congratulations, you are almost done!
instead of this you must call commands below to make integration with simulte
    mkdir build
    cd build
    cmake -DWITH_SIMULTE=ON ..
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
    
### Running 802.11p and LTE together 

SIMULTE scenario combines two interface card for heterogeneous communication 802.11p (V2V) and mobile LTE 
run this command on root directory to see the example work

cmake --build build/ --target run_simulte
    
    
## Create own ITS-G5 service (application ?? )

there was created an ExampleService located in src/artery/application that can be used as a reference one to create other services (applications ??). This service is running on the same level of the ITS-G5 stack : facilities.. on the same level CAN and DENM services are running. 
The service must be inherit from ItsG5Service and re-write his virtual methods. 
the trigger() method method is called every simulation step. (0.01 s). here you can check some parameters to send message (or not) using ITS_G5 protocol : BTP (transport) and GeoNet - SHB (SINGLE HOP BROADCAST) (network). this parameters can be configured accordingly 


Your service can subscribe to receive data from other services like CAM or DENM and use this data on your own application.. also you can obtain information about your vehicle (position speed etc) using traci::VehicleController variable.. in Example scenario we subscribe to receive data from CAM service and extract information previously decoding : 
        const vanetza::asn1::Cam& msg =  dynamic_cast <CaObject*>(obj)->asn1();

The CAM service create and maintain LCM (Local Dynamic Map) that also can be used in your application
check code example comments to understand better 

to switch on your service on simulation you must include the service class name and service port on artrery/scenarios/YOUR_SCENARIO/services.xml file like this 

	<service type="artery.application.ExampleService">
		<listener port="3003" />
	</service>

### ASN.1 encoding 

the BTP PDU and FSDU (facilities layer service data unit) description can be found on set of ETSI EN302... v1.3.2  documentations.. the standard regulates all aspects of using ITS-G5 protocol stack : for example [GeoNet](https://www.etsi.org/deliver/etsi_en/302600_302699/3026360401/01.03.01_60/en_3026360401v010301p.pdf)

for example heare is the [CAM-PDU-Descriptions.CAM](https://github.com/bastibl/its-g5-cam/blob/master/ITS_CAM_v1.3.2.asn)

for service you can create own asn.1 PDU description  than using 'asn1c' utility generate c++ classes and headers to include on your project.. can find some examples of how to use own encoding messages in vatenza framework folder (artery/extern/vanetza/vanetza/asn1/...)

### ETSI ITS G5 GeoNetworking stack, in Java 

the sollution for java developers can be found [here](https://github.com/alexvoronov/geonetworking) 

also this model can be used like a Model of running IPv6 over 802.11p

### Cohda MKx ETS-Shell application

[Cohda MKx ETS-Shell application](http://doxygen.cohdawireless.com/ets-shell/html/a00416_source.html)









