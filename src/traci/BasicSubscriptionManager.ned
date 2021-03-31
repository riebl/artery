package traci;

simple BasicSubscriptionManager like SubscriptionManager
{
    parameters:
        @class(traci::BasicSubscriptionManager);
        string coreModule;
        bool ignorePersons;
}
