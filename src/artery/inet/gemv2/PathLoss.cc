/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/inet/gemv2/LinkClassifier.h"
#include "artery/inet/gemv2/PathLoss.h"
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

void PathLoss::initialize()
{
    m_los = check_and_cast<IPathLoss*>(getSubmodule("LOS"));
    m_nlos_b = check_and_cast<IPathLoss*>(getSubmodule("NLOSb"));
    m_nlos_v = check_and_cast<IPathLoss*>(getSubmodule("NLOSv"));
    m_classifier = check_and_cast<LinkClassifier*>(getSubmodule("classifier"));
}

double PathLoss::computePathLoss(const phy::ITransmission* transmission, const phy::IArrival* arrival) const
{
    inet::Coord tx = transmission->getStartPosition();
    inet::Coord rx = arrival->getStartPosition();

    using Link = LinkClassifier::Link;
    Link link = m_classifier->classifyLink(Position { tx.x, tx.y }, Position { rx.x, rx.y });
    IPathLoss* model = nullptr;
    switch (link)
    {
        case Link::LOS:
            model = m_los;
            EV_DETAIL << "LOS propagation for " << *transmission << "\n";
            break;
        case Link::NLOSb:
            model = m_nlos_b;
            EV_DETAIL << "NLOSb propagation for " << *transmission << "\n";
            break;
        case Link::NLOSv:
            model = m_nlos_v;
            EV_DETAIL << "NLOSv propagation for " << *transmission << "\n";
            break;
        default:
            throw cRuntimeError("invalid link classification");
    };

    return model->computePathLoss(transmission, arrival);
}

double PathLoss::computePathLoss(mps, Hz, m) const
{
    throw omnetpp::cRuntimeError("Incompatible usage of GEMV2 path loss model");
    return 1.0;
}

m PathLoss::computeRange(mps, Hz, double loss) const
{
    // TODO: has GEMV2 a maximum range? Is it configurable?
    return m(NaN);
}

} // namespace gemv2
} // namespace artery
