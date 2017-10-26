/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef PATHLOSS_H_ZABKB47G
#define PATHLOSS_H_ZABKB47G

#include <inet/physicallayer/contract/packetlevel/IPathLoss.h>
#include <omnetpp/csimplemodule.h>

namespace artery
{
namespace gemv2
{

class PathLoss : public omnetpp::cSimpleModule, public inet::physicallayer::IPathLoss
{
public:
    // OMNeT++ simple module
    void initialize() override;

    // INET IPathLoss interface
    double computePathLoss(const inet::physicallayer::ITransmission*, const inet::physicallayer::IArrival*) const override;
    double computePathLoss(inet::mps, inet::Hz, inet::m) const override;
    inet::m computeRange(inet::mps, inet::Hz, double loss) const override;

private:
    inet::physicallayer::IPathLoss* m_los;
    inet::physicallayer::IPathLoss* m_nlos_b;
    inet::physicallayer::IPathLoss* m_nlos_v;
};

} // namespace gemv2
} // namespace artery

#endif /* PATHLOSS_H_ZABKB47G */

