Development of Artery's environment model (envmod) has been motivated by the Collective Perception use case.
This use case intends to share objects perceived by vehicles' sensors with other road participants via V2X communication.
The environment model has been presented first in 2015 [^1].

[^1]:
    Hendrik-Jörn Günther, Oliver Trauer, Lars Wolf: "The Potential of Collective Perception in Vehicular Ad-hoc Networks", International Conference on ITS Telecommunications (ITST), 2015 (DOI: [10.1109/ITST.2015.7377190](https://www.doi.org/10.1109/ITST.2015.7377190))

![sensor perception](../assets/envmod.gif)

## Global and local environment models

One `GlobalEnvironmentModel` tracks all obstacles and vehicles as reported by SUMO.
In the figure above, static obstacles such as buildings are coloured in dark green.
Vehicles, i.e. dynamic obstacles, are filled with blue.

Each vehicle with sensor capabilities possesses its `LocalEnvironmentModel`, which is the vehicle's local perspective on
the environment.
Similar to service modules attached to the middleware, the local environment model hosts a configurable set of sensors.
The XML configuration is very similar to the service configuration, and the same filter rules can be applied.

## Sensors and object perception

Sensors can be attached to the front, rear, left or right of a vehicle's body.
Services can get access via facilities to the local environment model and query it for perceived objects.

The range and field-of-view of radar sensors can be configured individually.
Only objects within the configured sensor cone can be perceived at all.
Furthermore, if line-of-sights to all corners of another vehicle are blocked, the perception of this vehicle is not possible.
Buildings and other vehicles can block these line-of-sights.
In environment model's visualisation, line-of-sights are drawn as dashed lines in the same colour as the corresponding sensor cone.
