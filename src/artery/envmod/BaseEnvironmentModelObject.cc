/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/BaseEnvironmentModelObject.h"
#include <omnetpp/cexception.h>

namespace artery
{

const Position& BaseEnvironmentModelObject::getAttachmentPoint(const SensorPosition& pos) const
{
    assert(mAttachmentPoints.size() == 4);
    const Position* point = nullptr;
    switch (pos) {
        case SensorPosition::FRONT:
            point = &mAttachmentPoints[0];
            break;
        case SensorPosition::BACK:
            point = &mAttachmentPoints[2];
            break;
        case SensorPosition::RIGHT:
            point = &mAttachmentPoints[1];
            break;
        case SensorPosition::LEFT:
            point = &mAttachmentPoints[3];
            break;
        default:
            throw omnetpp::cRuntimeError("Invalid sensor attachment point requested");
            break;
    }

    return *point;
}

} // namespace artery
