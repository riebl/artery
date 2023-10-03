Traffic Control Interface ([TraCi](https://sumo.dlr.de/docs/TraCI.html)) provides access to a running road traffic simulation in SUMO and allows to retrieve values of simulated objects and manipulate their behaviour online.

## Points of interest
- The `./Manager`-Module is used in `src/artery/veins/World.ned`
- The `./Core.ned`-Module is used in `./Manager`-Module
    - `./Core.ned`-Module which seems to transform TraCi API Responses into Omnet Signals
- `./BasicSubscriptionManager.ned`
