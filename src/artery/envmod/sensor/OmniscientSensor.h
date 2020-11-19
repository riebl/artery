/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_OMNISCIENTSENSOR_H_
#define ENVMOD_OMNISCIENTSENSOR_H_

#include "artery/envmod/sensor/FovSensor.h"

namespace artery
{

class OmniscientSensor : public FovSensor
{
public:
    void initialize() override;
    const std::string& getSensorCategory() const { return category; }

private:
    const std::string category = "Omniscient";
};



} // namespace artery

#endif /* ENVMOD_OMNISCIENTSENSOR_H_ */
