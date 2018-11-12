#ifndef POLICESERVICESTORYBOARD_H_
#define POLICESERVICESTORYBOARD_H_

#include "PoliceService.h"

// Extends PoliceService which should be used with the storyboard feature
class PoliceServiceStoryboard : public PoliceService
{
    public:
        void initialize() override;
        void trigger() override;
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;

    private:
        bool activatedSiren = false;
};

#endif /* POLICESERVICESTORYBOARD_H_ */
