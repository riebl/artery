/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef PATHLOSS_H_ZABKB47G
#define PATHLOSS_H_ZABKB47G

#include <inet/common/Units.h>
#include <inet/physicallayer/contract/packetlevel/IPathLoss.h>
#include <omnetpp/csimplemodule.h>

namespace artery
{
namespace gemv2
{

// forward declarations
class LinkClassifier;
class SmallScaleVariation;

class PathLoss : public omnetpp::cSimpleModule, public inet::physicallayer::IPathLoss
{
public:
    PathLoss();

    // OMNeT++ simple module
    void initialize() override;

    // INET IPathLoss interface
    double computePathLoss(const inet::physicallayer::ITransmission*, const inet::physicallayer::IArrival*) const override;
    double computePathLoss(inet::mps, inet::Hz, inet::m) const override;
    inet::m computeRange(inet::mps, inet::Hz, double loss) const override;

private:
    using meter = inet::m;

    inet::physicallayer::IPathLoss* m_los;
    inet::physicallayer::IPathLoss* m_nlos_b;
    inet::physicallayer::IPathLoss* m_nlos_f;
    inet::physicallayer::IPathLoss* m_nlos_v;
    LinkClassifier* m_classifier;
    SmallScaleVariation* m_small_scale;
    meter m_range_los;
    meter m_range_nlos_b;
    meter m_range_nlos_f;
    meter m_range_nlos_v;
};

} // namespace gemv2
} // namespace artery

#endif /* PATHLOSS_H_ZABKB47G */

