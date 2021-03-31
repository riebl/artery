#ifndef ARTERY_VEINSOBSTACLECONTROL_H_TFINXPHU
#define ARTERY_VEINSOBSTACLECONTROL_H_TFINXPHU

#include "traci/Listener.h"
#include <veins/modules/obstacle/ObstacleControl.h>

namespace traci { class API; }

namespace artery
{

class VeinsObstacleControl : public veins::ObstacleControl, public traci::Listener
{
public:
    void initialize(int stage) override;

private:
    void traciInit() override;
    void fetchObstacles(std::shared_ptr<traci::API>);
};

} // namespace artery

#endif /* ARTERY_VEINSOBSTACLECONTROL_H_TFINXPHU */

