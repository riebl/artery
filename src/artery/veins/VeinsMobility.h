#ifndef VEINSMOBILITY_H_JFWG67L1
#define VEINSMOBILITY_H_JFWG67L1

#include "artery/traci/MobilityBase.h"
#include <veins/base/modules/BaseMobility.h>
#include <veins/base/utils/Coord.h>

class VeinsMobility : public BaseMobility /* Veins */, public MobilityBase /* Artery */
{
public:
    void initialize(int stage) override;

private:
    void update(const Position&, Angle, double speed) override;

    Coord mPosition;
    Coord mDirection;
    double mSpeed;
};

#endif /* VEINSMOBILITY_H_JFWG67L1 */

