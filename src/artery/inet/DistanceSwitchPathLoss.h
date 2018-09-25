/*
* Artery V2X Simulation Framework
* Copyright 2018 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_DISTANCESWITCHPATHLOSS_H_G8PI60NO
#define ARTERY_DISTANCESWITCHPATHLOSS_H_G8PI60NO

#include <inet/common/Units.h>
#include <inet/physicallayer/contract/packetlevel/IPathLoss.h>

namespace artery
{

class DistanceSwitchPathLoss : public omnetpp::cSimpleModule, public inet::physicallayer::IPathLoss
{
public:
    void initialize() override;
    double computePathLoss(const inet::physicallayer::ITransmission*, const inet::physicallayer::IArrival*) const override;
    double computePathLoss(inet::mps propagation, inet::Hz frequency, inet::m distance) const override;
    inet::m computeRange(inet::mps propagation, inet::Hz frequency, double loss) const override;

private:
    inet::m mThreshold;
    inet::physicallayer::IPathLoss* mModelNear;
    inet::physicallayer::IPathLoss* mModelFar;
};

} // namespace artery

#endif /* ARTERY_DISTANCESWITCHPATHLOSS_H_G8PI60NO */
