package artery.storyboard;

simple Storyboard
{
    parameters:
        @display("i=block/blackboard;is=s");
        @class(Storyboard);
        string python = default("storyboard");
        string middlewareModule = default(".middleware");
        string traciModule = default("^.traci");
        string canvas = default("storyboard");
        bool drawConditions = default(true);
}
