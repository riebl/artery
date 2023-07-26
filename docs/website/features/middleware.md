# Middleware

The middleware is one of the modules already found in the earliest Artery versions.[^1]
It acts as a central hub for V2X services running on each vehicle.
In no particular order, the responsibilities of the middleware comprise:

- lifecycle of V2X service modules
- dispatch of V2X messages between lower layers and services
- facilities shared among services

[^1]:
    While the idea behind the middleware has remained over the years, it has evolved quite a lot over the years.
    The first published paper about Artery covers the features of this early middleware extensively:  
    Raphael Riebl, Hendrik-Jörn Günther, Christian Facchi, Lars Wolf: "Artery – Extending Veins for VANET applications", Models and Technologies for Intelligent Transportation Systems (MT-ITS), 2015 (DOI: [10.1109/MTITS.2015.7223293](https://doi.org/10.1109/MTITS.2015.7223293))


## Services

Services are generated dynamically by the middleware during its initialisation.
A module can be employed as a service if it derives from the `ItsG5BaseService` class.
Classes inheriting from `ItsG5BaseService` are perfectly fine as well, e.g. `ItsG5Service` or `ItsG5PromiscousService`.
Latter is a special class receiving any incoming packets irrespective of the port number.

### Deployment configuration

Middleware's *services* expects a XML configuration, usually given as a XML file by OMNeT++'s `xmldoc` function.
For each service used in the simulation setup, this XML configuration contains a `<service>` tag.
At minimum, this tag has a `type` attribute referring to the OMNeT++ module type realising the particular service.
How this service is named can be set by the optional `name` attribute.
If unset, the `type` will be used as name as well, but it might be a bit lengthy.

Services are primarily intended as end-points of V2X communication, i.e. they are the applications in terms of a network stack.
One or more `<listener>` tags specify on which BTP port and which radio channel the respective service listens for incoming messages.
While the `port` attribute is mandatory for usual services, the `channel` attribute defaults to the G5-CCH channel (180).

The `<filters>` section of a `<service>` is an optional but powerful mechanism.
Only if its filter section evaluates to true, the station will be equipped with the associated service.

| Filter            | Attribute | Value                     | Effect                                                    |
|-------------------|-----------|---------------------------|-----------------------------------------------------------|
| `<name>`          | pattern   | a regular expression      | vehicle name (SUMO ID) has to match given expression      |
|                   | match     | 'inverse'                 | name shall NOT match given expression                     |
| `<penetration>`   | rate      | number from 0.0 to 1.0    | only given percentage will be equipped (Bernoulli trial)  |
| `<type>`          | pattern   | a regular expression      | host module type has to match given expression            |
|                   | match     | 'inverse'                 | module type shall NOT match given expression              |
|                   | rate      | number from 0.0 to 1.0    | only given percentage of this module type                 |


### Periodic updates

Each service will be triggered by the middleware periodically.
The interval between these triggering events is configurable by middleware's *updateInterval* parameter.
All services belonging to the same station will get triggered at the same instant of time.
However, among all middleware instances, a random jitter is introduced, so it is unlikely that multiple stations will be triggering their services synchronously.
This jitter avoids artificially aligned behaviour among stations, which could be harmful.
For example, when services generate messages for transmission upon the trigger event, an artificially high risk of packet collisions would be caused without this jitter.
Services are not obliged to use this trigger mechanism at all.
It is not uncommon when service modules schedule OMNeT++ messages for individual timings.

### Sending and receiving V2X messages

Services can request a transmission by calling their `request` method.
This method expects a BTP data request (a structure from Vanetza) and the packet payload.
Either an OMNeT++ `cPacket` or a Vanetza packet object can be passed as payload.

On packet receptions, the middleware will call the `indicate` method of the service matching the port number.
Transmissions can be generated for any port number independent of the configured `<listener>`, though.
Besides the received packet payload, a BTP data indication will be passed to service as well.


## Facilities

Each middleware is equipped with a `Facilities` object, which is a universal, typed object container.
This mechanism allows to register arbitrary C++ objects in one module and retrieve access to them in another module.
Usually, the middleware registers several utility objects during its initialisation phase and services make use of them.
Services may register their own objects as well, though, if they shall be shared among sibling services, for example.

Typical code lines found in service classes retrieving objects from facilities look like this:

```cpp
// will throw an exception if now Timer has been registered before
const Timer& timer = getFacilities().get_const<Timer>();
// will return nullptr if no (mutable) LocalDynamicMap has been registered before
LocalDynamicMap* ldm = getFacilities().get_mutable_ptr<LocalDynamicMap>();
```


### Date and Time

The *datetime* parameter of the middleware module accepts date + time strings, e.g. "2017-10-26 15:05:00".
This time point corresponds to the start time *t = 0s* of an OMNeT++ simulation.
Whenever the Vanetza ITS-G5 stack needs to include a timestamp in a protocol data unit, it derives this timestamp from the current simulation time and the configured *datetime* epoch.
Also, an instance of `Timer` is accessible from the facilities.
This timer enables easy conversion between OMNeT++ simulation time and ITS time.


### Identity

Several identifier exist across protocol layers, which refer to the same station (vehicle, roadside unit, etc.).
Those modules of a host "owning" an identifier can store it in the `Identity` object.
In combination with a global `IdentityRegistry`, one can quickly look up which OMNeT++ host module belongs to a station ID included in a Cooperative Awareness Message (CAM) or a particular SUMO vehicle ID.

### Local Dynamic Map

Packets are fugitive by nature, i.e. they vanish after being delivered to the receiving service.
This behaviour is sufficient for some use cases, for example, where the reception triggers a reaction immediately.
As its name suggests, Cooperative Awareness aims at increasing the awareness of nearby vehicles.
The `LocalDynamicMap` stores received CAMs for some time to enable queries regarding neighbouring stations.

!!! example
    According to the triggering conditions defined by the CAR 2 CAR Communication Consortium, a traffic jam is detected by counting nearby slow vehicles, among others.
    This can be realised by querying the `LocalDynamicMap` attached to each middleware.

### Vehicle Data Provider

Many services require access to vehicle data, e.g. position or speed.
With `VehicleDataProvider` exists a generic object granting access to these data fields, irrespective of the source.
A source can be a SUMO vehicle, or an OTS GTU, or yet another source providing `VehicleKinematics`.
If a source lacks some data field, e.g. SUMO does not report acceleration and yaw rate, the vehicle data provider calculates this data by differentiating speed or heading.
Additionally, the vehicle data provider calculates the current curvature of the travelled path.

### Vehicle Controller

Vehicles are not mere data sources, but in fact a "bi-directional coupling" between the communication network and the road network exists.
If a middleware's host is backed by a SUMO vehicle, one can retrieve a `traci::VehicleController` instance.
Such a controller object enables services to influence a vehicle's behaviour, e.g. by changing its speed profile or route.
