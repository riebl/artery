/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef BASEENVIRONMENTMODELOBJECT_H_
#define BASEENVIRONMENTMODELOBJECT_H_

#include "artery/envmod/EnvironmentModelObject.h"

namespace artery
{

/**
 * Base class for environment model objects implementing a few
 * getters backed by member variables. The class is still abstract.
 */
class BaseEnvironmentModelObject : public EnvironmentModelObject
{
public:
    const std::vector<Position>& getOutline() const { return mOutline; }
    const Position& getAttachmentPoint(const SensorPosition& pos) const;
    const Position& getCentrePoint() const { return mCentrePoint; }
    Length getLength() const override { return mLength; }
    Length getWidth() const override { return mWidth; }
    Length getRadius() const override { return mRadius; }

protected:
    Length mLength;
    Length mWidth;
    Length mRadius;
    std::vector<Position> mOutline;
    std::vector<Position> mAttachmentPoints;
    Position mCentrePoint;
};

} // namespace artery

#endif /* BASEENVIRONMENTMODELOBJECT_H_ */
