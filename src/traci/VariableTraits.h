/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef VARIABLETRAITS_H_LZ4RYGAV
#define VARIABLETRAITS_H_LZ4RYGAV

#include "traci/sumo/traci-server/TraCIConstants.h"

namespace traci
{

template<int VAR>
struct VariableTrait;

#define VAR_TRAIT(var_, type_) \
    template<> struct VariableTrait<var_> { using value_type = type_; };

VAR_TRAIT(VAR_SPEED, double)
VAR_TRAIT(VAR_POSITION, TraCIPosition)
VAR_TRAIT(VAR_ANGLE, double)
VAR_TRAIT(VAR_MAXSPEED, double)
VAR_TRAIT(VAR_TYPE, std::string)
VAR_TRAIT(VAR_VEHICLECLASS, std::string)
VAR_TRAIT(VAR_LENGTH, double)
VAR_TRAIT(VAR_WIDTH, double)
VAR_TRAIT(VAR_ARRIVED_VEHICLES_IDS, std::vector<std::string>)
VAR_TRAIT(VAR_DEPARTED_VEHICLES_IDS, std::vector<std::string>)
VAR_TRAIT(VAR_TIME_STEP, SUMOTime)
VAR_TRAIT(VAR_SIGNALS, int)

#undef VAR_TRAIT

} // namespace traci

#endif /* VARIABLETRAITS_H_LZ4RYGAV */

