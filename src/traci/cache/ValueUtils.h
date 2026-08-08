/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Copyright 2026 Narkhov Evgeny
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#pragma once

#include <omnetpp/cexception.h>
#include <traci/API.h>
#include <traci/cache/VariableTraits.h>

#include <memory>
#include <type_traits>
#include <typeinfo>

namespace artery {

namespace traci {

/**
 * @brief Convert cpp value to Traci Result.
 */
template <typename T>
auto makeValue(T variable) -> typename TraCIResultTrait<std::decay_t<T>>::result_type;

/**
 * @brief Convert Traci Result to cpp value. For generic Traci Result type, use @ref getValue.
 */
template <typename T>
auto getValueImpl(const typename TraCIResultTrait<std::decay_t<T>>::result_type& variable) -> T;

/**
 * @brief Get cpp value (may be a reference type) from Traci Result. Make
 * Sure that @ref getValueImpl returns a reference type if template parameter is
 * a reference.
 */
template <typename T>
auto getValue(const lib::TraCIResult& result) -> T {
    using decayed_type = std::decay_t<T>;
    using result_type = typename TraCIResultTrait<decayed_type>::result_type;

    if (auto* ptr = dynamic_cast<const result_type*>(result); !ptr) {
        const std::type_info& info = typeid(result_type);
        throw omnetpp::cRuntimeError("type mismatch: expected result to be %s", info.name());
    } else {
        return getValueImpl(*ptr);
    }
}

/**
 * @brief Similar to @ref getValue, but accepts a pointer instead.
 * @see getValue
 */
template <typename T>
auto getValueFromPointer(const std::shared_ptr<const lib::TraCIResult>& result) -> T {
    if (!result) {
        throw omnetpp::cRuntimeError("Null pointer passed");
    }
    return getValue<T>(*result);
}

}  // namespace traci

}  // namespace artery
