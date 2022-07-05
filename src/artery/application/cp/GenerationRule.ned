//
// Artery V2X Simulation Framework
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.application.cp;


// Interface of CpService Generation Rules
moduleinterface GenerationRule
{
}


// Generate CPM at fixed intervals
simple StaticGeneration like GenerationRule
{
    parameters:
        @class(cp::StaticGeneration);
}


// Generate CPM depending on dynamic properties of known Objects
simple DynamicGeneration like GenerationRule
{
    parameters:
        @class(cp::DynamicGeneration);

        // Distance difference between last transmitted and current position
        // that needs to get exceeded for inclusion
        double distanceDeltaThreshold @unit(m) = default(4m);
        // Speed difference between last transmitted and current speed
        // that needs to get exceeded for inclusion
        double speedDeltaThreshold @unit(mps) = default(0.5mps);
        // Heading-Angle difference in degree between last transmitted and current heading-angle
        // that needs to get exceeded for inclusion
        double headingDeltaThreshold = default(4.0);
        // Time difference between last transmitted and current time
        // that needs to get exceeded for inclusion
        int timeDeltaThreshold @unit(ms) = default(1000ms);
}


// Generate CPM depending on dynamic properties of known Objects with prediction
simple DynamicPredictionGeneration extends DynamicGeneration
{
    parameters:
        // If true, use the predicted transmission time of the next CPM to calculate
        // the future dynamic properties, otherwise use a fixed interval
        bool dynamicPrediction = default(false);
        // Fixed interval to predic the future dynamic properties for
        int predictionInterval @unit(ms) = default(100ms);
}
