/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Copyright 2026 Narkhov Evgeny
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "ValueUtils.h"

#include <libsumo/TraCIDefs.h>

using namespace artery;

template <>
auto traci::getValueImpl(const lib::TraCIInt& variable) -> int {
    return variable.value;
}

template <>
auto traci::getValueImpl(const lib::TraCIDouble& variable) -> double {
    return variable.value;
}

template <>
auto traci::getValueImpl(const lib::TraCIColor& variable) -> const lib::TraCIColor& {
    return variable;
}

template <>
auto traci::getValueImpl(const lib::TraCIPosition& variable) -> const lib::TraCIPosition& {
    return variable;
}

template <>
auto traci::getValueImpl(const lib::TraCIStringList& variable) -> const std::vector<std::string>& {
    return variable.value;
}

template <>
auto traci::getValueImpl(const lib::TraCIString& variable) -> const std::string& {
    return variable.value;
}

template <>
auto traci::makeValue(int variable) -> typename TraCIResultTrait<int>::result_type {
    return lib::TraCIInt(variable);
}

template <>
auto traci::makeValue(double variable) -> typename TraCIResultTrait<double>::result_type {
    return lib::TraCIDouble(variable);
}

template <>
auto traci::makeValue(const lib::TraCIPosition& variable) -> typename TraCIResultTrait<lib::TraCIPosition>::result_type {
    return variable;
}

template <>
auto traci::makeValue(const lib::TraCIColor& variable) -> typename TraCIResultTrait<lib::TraCIColor>::result_type {
    return variable;
}

template <>
auto traci::makeValue(const std::string& variable) -> typename TraCIResultTrait<std::string>::result_type {
    return lib::TraCIString(variable);
}

template <>
auto traci::makeValue(std::string&& variable) -> typename TraCIResultTrait<std::string>::result_type {
    return lib::TraCIString(std::move(variable));
}

template <>
auto traci::makeValue(const std::vector<std::string>& variable) -> typename TraCIResultTrait<std::vector<std::string>>::result_type {
    lib::TraCIStringList out;
    out.value = variable;
    return out;
}

template <>
auto traci::makeValue(std::vector<std::string>&& variable) -> typename TraCIResultTrait<std::vector<std::string>>::result_type {
    lib::TraCIStringList out;
    out.value = std::move(variable);
    return out;
}
