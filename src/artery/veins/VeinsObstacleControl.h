#ifndef VEINSOBSTACLECONTROL_H_TFINXPHU
#define VEINSOBSTACLECONTROL_H_TFINXPHU

#include "traci/Listener.h"
#include <veins/modules/obstacle/ObstacleControl.h>

namespace traci { class LiteAPI; }


class VeinsObstacleControl : public Veins::ObstacleControl, public traci::Listener
{
public:
    void initialize(int stage) override;

private:
    void traciInit() override;
    void fetchObstacles(traci::LiteAPI&);
};

#endif /* VEINSOBSTACLECONTROL_H_TFINXPHU */

