#ifndef POLICESERVICEENVMOD_H_L3L0PGCV
#define POLICESERVICEENVMOD_H_L3L0PGCV

#include "PoliceService.h"

// forward declaration
namespace artery { class LocalEnvironmentModel; }

// Extends PoliceService by using envmod features
class PoliceServiceEnvmod : public PoliceService
{
    public:
        void initialize() override;
        void trigger() override;

    private:
        const artery::LocalEnvironmentModel* localEnvmod;
        unsigned skippedTrigger;
};

#endif /* POLICESERVICEENVMOD_H_L3L0PGCV */
