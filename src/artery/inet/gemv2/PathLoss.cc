/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/inet/gemv2/PathLoss.h"
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
}

double PathLoss::computePathLoss(const phy::ITransmission* transmission, const phy::IArrival* arrival) const
{
    // TODO: choose model by link classification
    return m_los->computePathLoss(transmission, arrival);
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
