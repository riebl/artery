#ifndef POLICESERVICEEXTENDED_H_
#define POLICESERVICEEXTENDED_H_

#include "PoliceService.h"

// Extende PoliceService which should be used with the storyboard feature
class PoliceServiceExtended : public PoliceService
{
    public:
        void initialize() override;
        void trigger() override;
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;

    private:
        bool isTriggered = false;
};

#endif /* POLICESERVICE_H_ */
