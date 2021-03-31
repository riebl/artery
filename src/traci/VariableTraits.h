/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef VARIABLETRAITS_H_LZ4RYGAV
#define VARIABLETRAITS_H_LZ4RYGAV

#include "traci/sumo/libsumo/TraCIConstants.h"

namespace traci
{

template<int VAR>
struct VariableTrait;

template<typename T>
struct TraCIResultTrait;


#define RESULT_TRAIT(native_, traci_) \
    template<> struct TraCIResultTrait<native_> { using result_type = traci_; };

RESULT_TRAIT(double, libsumo::TraCIDouble)
RESULT_TRAIT(int, libsumo::TraCIInt)
RESULT_TRAIT(libsumo::TraCIColor, libsumo::TraCIColor)
RESULT_TRAIT(libsumo::TraCIPosition, libsumo::TraCIPosition)
RESULT_TRAIT(std::string, libsumo::TraCIString)
RESULT_TRAIT(std::vector<std::string>, libsumo::TraCIStringList)
#undef RESULT_TRAIT


#define VAR_TRAIT(var_, type_) \
    template<> struct VariableTrait<var_> { \
        using value_type = type_; \
        using result_type = TraCIResultTrait<value_type>::result_type; \
    };

VAR_TRAIT(libsumo::VAR_SPEED, double)
VAR_TRAIT(libsumo::VAR_POSITION, libsumo::TraCIPosition)
VAR_TRAIT(libsumo::VAR_ANGLE, double)
VAR_TRAIT(libsumo::VAR_MAXSPEED, double)
VAR_TRAIT(libsumo::VAR_TYPE, std::string)
VAR_TRAIT(libsumo::VAR_VEHICLECLASS, std::string)
VAR_TRAIT(libsumo::VAR_LENGTH, double)
VAR_TRAIT(libsumo::VAR_WIDTH, double)
VAR_TRAIT(libsumo::VAR_ARRIVED_VEHICLES_IDS, std::vector<std::string>)
VAR_TRAIT(libsumo::VAR_DEPARTED_VEHICLES_IDS, std::vector<std::string>)
VAR_TRAIT(libsumo::VAR_DELTA_T, double)
VAR_TRAIT(libsumo::VAR_TELEPORT_STARTING_VEHICLES_IDS, std::vector<std::string>)
VAR_TRAIT(libsumo::VAR_TIME, double)
VAR_TRAIT(libsumo::VAR_TIME_STEP, int)
VAR_TRAIT(libsumo::VAR_SIGNALS, int)
VAR_TRAIT(libsumo::VAR_ARRIVED_PERSONS_IDS, std::vector<std::string>)
VAR_TRAIT(libsumo::VAR_DEPARTED_PERSONS_IDS, std::vector<std::string>)
#undef VAR_TRAIT

} // namespace traci

#endif /* VARIABLETRAITS_H_LZ4RYGAV */

