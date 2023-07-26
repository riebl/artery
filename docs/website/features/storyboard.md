Usually, traffic scenarios modelled with SUMO are rather "static".
Though vehicles are moving around, traffic density varies and also traffic jams may build up, SUMO has no notion of sudden changes.
However, V2X use cases are often depending on sudden changes, e.g. unexpected heavy rain slowing down traffic in one region of the map.
Another example are accidents or vehicle breakdowns.
Those quite dynamic changes in the environment can be modelled with Artery's storyboard feature [^1].

[^1]:
    Christina Obermaier, Raphael Riebl, Christian Facchi: "Dynamic Scenario Control for VANET Simulations", Models and Technologies for Intelligent Transportation Systems (MT-ITS), 2017 (DOI: [10.1109/MTITS.2017.8005599](https://doi.org/10.1109/MTITS.2017.8005599))

The storyboard module integrates the Python interpreter and loads a storyboard script, supplied by the scenario designer.
This script needs to provide the Python function `createStories(board)`, which the storyboard will invoke at the beginning.
Multiple stories can be registered via this script, each consisting of conditions and effects.


## Conditions

Conditions of a story need to be fulfilled, so that its associated effects get applied.
Most conditions evaluate some runtime property and turn true if this property is within given bounds.

| Condition | Parameters | Fulfilled when... |
| --- | --- | --- |
| TimeCondition | begin, [end] | ... simulation time is within given bounds (begin; end) |
| SpeedCondition[Greater, Less] | mps | ... vehicle speed is above/below (mps) |
| CarSetCondition | IDs | ... vehicle ID matches one of given IDs |
| PolygonCondition | Positions | ... vehicle is within defined polygon |
| SpeedDifferenceCondition[Faster, Slower] | delta_mps | ... at least one neighbouring vehicles being faster/slower by (delta_mps) has been selected |
| TtcCondition | ttc, [distance] | ... calculated time-to-collision falls below (ttc). Vehicles up to (distance) meters away are considered. |

Furthermore, with `AndCondition` and `OrCondition` two conditions can be logically combined.
Combinations of these allow to build up entire condition trees.
Conditions trees are evaluated lazily, i.e. branches are only evaluated if conditions of upper nodes are true.
Lazy evaluation can be combined with `LimitCondition`, which is only true for the first N vehicles.

For example, assume a condition tree `AndCondition(PolygonCondition(...), LimitCondition(3))`.
This tree will be only true for the first three vehicles entering the area spanned by the polygon.


## Effects

Each story is associated with one or more effects.
These effects are applied to the matching vehicle as long as the conditions are fulfilled.
If multiple effects, possibly by distinct stories, are applied at the same time, the storyboard will manage the correct order of them.
Storyboard effect stack ensures a valid restoration of the affected properties when the story becomes inactive again.

| Name | Parameters | Effect |
| --- | --- | --- |
| SpeedEffect | mps | Enforces a maximum speed (mps) of the vehicle |
| StopEffect | - | Stops vehicle entirely |
| SignalEffect | name | Emits a `StoryboardSignal` containing (name) |

The `SignalEffect` is likely the most versatile of them.
Services may listen for emitted storyboard signals and react upon them according to the stored string.
The sky is the limit...
