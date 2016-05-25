package org.car2x.veins.nodes;

import artery.storyboard.Storyboard;
import org.car2x.veins.nodes.Scenario;

network StoryboardScenario extends Scenario
{
    submodules:
        storyboard: Storyboard {
            parameters:
                @display("p=65,82");
        }
}