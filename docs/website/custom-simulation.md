# Create a custom simulation setup

After [installing](install.md) Artery, you are now eager to create your own simulation?
This guide takes you step-by-step through creating a new [scenario](#create-a-new-scenario) and a new [service](#create-a-new-service).

## Create a new scenario

1. Create new scenario folder in *scenario/* such as *scenario/your_scenario*
2. Create and edit necessary files for SUMO (cf. [SUMO documentation](https://sumo.dlr.de/docs/Other/File_Extensions.html))
    - SUMO config: *your_scenario.sumocfg*
    - road network file: *your_scenario.net.xml*
    - routes file: *your_scenario.rou.xml*
3. Create and edit necessary files for Artery/OMNeT++
    - add run target for your scenario to *scenario/your_scenario/CMakeLists.txt*  
      `add_opp_run(your_scenario CONFIG omnetpp.ini)`  
      This line enables the *run_your_scenario* and *debug_your_scenario* targets.
    - add a OMNeT++ configuration file  
      Set `*.traci.launcher.sumocfg = your_scenario.sumocfg` so Artery launches your SUMO scenario.
      Have a look at *scenarios/artery/omnetpp.ini* which showcases many of Artery's features.
    - *services.xml* specifies the [services](features/middleware.md#services) the vehilces will run, e.g. `CaService` or `Your_Service`
    - *sensors.xml* if you intend to use the [environment model](features/envmod.md)
4. Add your scenario folder to *scenario/CMakeLists.txt*  
   `add_subdirectory(your_scenario)`

With these files in place, you can now build and run your scenario with `cmake --build build --target run_your_scenario`.


## Create a new service

At first, have a look at *ExampleService* to familiarize yourself with the basic structure of an Artery service application.
You can find its header (*ExampleService.h*), source file (*EmapleService.cc*), and OMNeT++ module description (*ExampleService.ned*) in the *src/artery/application* directory.

1. Create and edit your service in *src/artery/application/*
    - C++ files: *Your_Service.cc* and *Your_Service.h*
    - OMNeT++ message files (optional): *Your_Message.msg* (cf. [OMNeT++ documentation](https://doc.omnetpp.org/omnetpp/manual/#cha:msg-def))
    - OMNeT++ module file: *Your_Service.ned*
2. Add your C++ sources to *src/artery/CMakeLists.txt* just like *CaService.cc* and consorter
3. That's it!

While placing all your services in *src/artery/application* is fine, this is not a strict requirement.
For example, *scenarios/highway-police* demonstrates how to add a service directly next to a scenario.
Furthermore, our services related to the environment model are located at *src/artery/envmod/service*.


!!! note "Credits"
    Thanks to [Alexander Willecke](https://github.com/awillecke) for contributing this page.
