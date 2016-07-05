package artery.storyboard.nodes;

import artery.storyboard.Storyboard;
import org.car2x.veins.nodes.Scenario;

network StoryboardScenario extends Scenario
{
    parameters:
        traciScenarioManager = "TraCIScenarioManagerStoryboard";

    submodules:
        storyboard: Storyboard {
            parameters:
                @display("p=65,82");
        }
}
