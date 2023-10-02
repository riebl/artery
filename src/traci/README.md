Traffic Control Interface ([TraCi](https://sumo.dlr.de/docs/TraCI.html)) ermöglicht den Zugriff auf eine laufende Straßenverkehrssimulation in SUMO und erlaubt es, Werte von simulierten Objekten abzurufen und deren Verhalten online zu manipulieren.

## Points of interest
- The `./Manager`-Module is used in `src/artery/veins/World.ned`
- The `./Core.ned`-Module is used in `./Manager`-Module
    - `./Core.ned`-Module which seems to transform TraCi API Responses into Omnet Signals
- `./BasicSubscriptionManager.ned`