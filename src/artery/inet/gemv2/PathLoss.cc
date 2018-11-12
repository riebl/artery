/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/inet/gemv2/LinkClassifier.h"
#include "artery/inet/gemv2/PathLoss.h"
#include "artery/inet/gemv2/SmallScaleVariation.h"
#include "artery/utility/Geometry.h"
#include <omnetpp/checkandcast.h>
#include <omnetpp/cexception.h>

namespace artery
{
namespace gemv2
{

Define_Module(PathLoss)

using namespace inet;
namespace phy = inet::physicallayer;

PathLoss::PathLoss() :
    m_los(nullptr), m_nlos_b(nullptr), m_nlos_f(nullptr), m_nlos_v(nullptr),
    m_classifier(nullptr), m_small_scale(nullptr),
    m_range_los(NaN), m_range_nlos_b(NaN), m_range_nlos_f(NaN), m_range_nlos_v(NaN)
{
}

void PathLoss::initialize()
{
    m_los = check_and_cast<IPathLoss*>(getSubmodule("LOS"));
    m_nlos_b = check_and_cast<IPathLoss*>(getSubmodule("NLOSb"));
    m_nlos_f = check_and_cast<IPathLoss*>(getSubmodule("NLOSf"));
    m_nlos_v = check_and_cast<IPathLoss*>(getSubmodule("NLOSv"));
    m_classifier = check_and_cast<LinkClassifier*>(getSubmodule("classifier"));
    m_small_scale = dynamic_cast<SmallScaleVariation*>(getSubmodule("smallScaleVariations"));
    m_range_los = meter(par("rangeLOS"));
    m_range_nlos_b = meter(par("rangeNLOSb"));
    m_range_nlos_f = meter(par("rangeNLOSf"));
    m_range_nlos_v = meter(par("rangeNLOSv"));
}

double PathLoss::computePathLoss(const phy::ITransmission* transmission, const phy::IArrival* arrival) const
{
    inet::Coord tx = transmission->getStartPosition();
    inet::Coord rx = arrival->getStartPosition();

    LinkClass link = m_classifier->classifyLink(Position { tx.x, tx.y }, Position { rx.x, rx.y });
    IPathLoss* model = nullptr;
    meter range { 0.0 };
    switch (link)
    {
        case LinkClass::LOS:
            model = m_los;
            range = m_range_los;
            EV_DETAIL << "LOS propagation for " << *transmission << "\n";
            break;
        case LinkClass::NLOSb:
            model = m_nlos_b;
            range = m_range_nlos_b;
            EV_DETAIL << "NLOSb propagation for " << *transmission << "\n";
            break;
        case LinkClass::NLOSf:
            model = m_nlos_f;
            range = m_range_nlos_f;
            EV_DETAIL << "NLOSf propagation for " << *transmission << "\n";
            break;
        case LinkClass::NLOSv:
            model = m_nlos_v;
            range = m_range_nlos_v;
            EV_DETAIL << "NLOSv propagation for " << *transmission << "\n";
            break;
        default:
            throw cRuntimeError("invalid link classification");
    };

    // compare model's maximum range with actual distance
    if (tx.distance(rx) > range.get()) {
        return 0.0; // all signal power is lost
    }

    double loss = model->computePathLoss(transmission, arrival);
    if (m_small_scale) {
        loss *= m_small_scale->computeVariation(Position { tx.x, tx.y }, Position { rx.x, rx.y }, range, link);
    }
    return loss;
}

double PathLoss::computePathLoss(mps, Hz, m) const
{
    throw omnetpp::cRuntimeError("Incompatible usage of GEMV2 path loss model");
    return 1.0;
}

m PathLoss::computeRange(mps, Hz, double loss) const
{
    return std::max({m_range_los, m_range_nlos_b, m_range_nlos_v});
}

} // namespace gemv2
} // namespace artery
