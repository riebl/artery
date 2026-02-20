/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Copyright 2026 Narkhov Evgeny
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#pragma once

#include <traci/API.h>

/**
 * @file This file provides type traits for default cpp types and libsumo/libtraci result
 * types to enable simple two-way conversions.
 */

namespace artery {

namespace traci {

template <typename T>
struct TraCIResultTrait;

namespace detail {

template <typename Exported = lib::TraCIResult>
struct TraCIResultImpl {
    using result_type = Exported;
};

template <typename VariableType = lib::TraCIResult>
struct VariableImpl {
    using value_type = VariableType;
    using result_type = typename TraCIResultTrait<VariableType>::result_type;
};

}  // namespace detail

/**
 * @brief A match between a cpp type and a result type.
 */
template <typename T>
struct TraCIResultTrait : detail::TraCIResultImpl<> {};

/*************************
 * Result Specifications *
 *************************
 */

template <>
struct TraCIResultTrait<double> : detail::TraCIResultImpl<lib::TraCIDouble> {};
template <>
struct TraCIResultTrait<int> : detail::TraCIResultImpl<lib::TraCIInt> {};
template <>
struct TraCIResultTrait<std::string> : detail::TraCIResultImpl<lib::TraCIString> {};
template <>
struct TraCIResultTrait<std::vector<std::string>> : detail::TraCIResultImpl<lib::TraCIStringList> {};
template <>
struct TraCIResultTrait<lib::TraCIColor> : detail::TraCIResultImpl<lib::TraCIColor> {};
template <>
struct TraCIResultTrait<lib::TraCIPosition> : detail::TraCIResultImpl<lib::TraCIPosition> {};

/**
 * @brief A match between a variable (its integer id) and its plain cpp type.
 */
template <int V>
struct VariableTrait : public detail::VariableImpl<> {};

/***************************
 * Variable Specifications *
 ***************************
 */

template <>
struct VariableTrait<lib::VAR_SPEED> : detail::VariableImpl<double> {};
template <>
struct VariableTrait<lib::VAR_POSITION> : detail::VariableImpl<lib::TraCIPosition> {};
template <>
struct VariableTrait<lib::VAR_ANGLE> : detail::VariableImpl<double> {};
template <>
struct VariableTrait<lib::VAR_MAXSPEED> : detail::VariableImpl<double> {};
template <>
struct VariableTrait<lib::VAR_TYPE> : detail::VariableImpl<std::string> {};
template <>
struct VariableTrait<lib::VAR_VEHICLECLASS> : detail::VariableImpl<std::string> {};
template <>
struct VariableTrait<lib::VAR_VEHICLE> : detail::VariableImpl<std::string> {};
template <>
struct VariableTrait<lib::VAR_LENGTH> : detail::VariableImpl<double> {};
template <>
struct VariableTrait<lib::VAR_WIDTH> : detail::VariableImpl<double> {};

template <>
struct VariableTrait<lib::VAR_ARRIVED_VEHICLES_IDS> : detail::VariableImpl<std::vector<std::string>> {};
template <>
struct VariableTrait<lib::VAR_DEPARTED_VEHICLES_IDS> : detail::VariableImpl<std::vector<std::string>> {};
template <>
struct VariableTrait<lib::VAR_DELTA_T> : detail::VariableImpl<double> {};
template <>
struct VariableTrait<lib::VAR_TELEPORT_STARTING_VEHICLES_IDS> : detail::VariableImpl<std::vector<std::string>> {};
template <>
struct VariableTrait<lib::VAR_TIME> : detail::VariableImpl<double> {};
template <>
struct VariableTrait<lib::VAR_TIME_STEP> : detail::VariableImpl<int> {};
template <>
struct VariableTrait<lib::VAR_SIGNALS> : detail::VariableImpl<int> {};
template <>
struct VariableTrait<lib::VAR_ARRIVED_PERSONS_IDS> : detail::VariableImpl<std::vector<std::string>> {};
template <>
struct VariableTrait<lib::VAR_DEPARTED_PERSONS_IDS> : detail::VariableImpl<std::vector<std::string>> {};

}  // namespace traci

}  // namespace artery
