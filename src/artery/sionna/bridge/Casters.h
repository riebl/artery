#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/Fwd.h>
#include <artery/sionna/bridge/capabilities/Core.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/variant.h>
#include <nanobind/stl/vector.h>

namespace artery::sionna::py
{

/*****************
 * Caster Traits *
 *****************
 */

// Resolves to true if T supports all capabilities required by wrapper caster.
template <typename T>
struct sionna_wrap_caster_enabled : supports_capabilities<T, ExportBoundObjectCapability, IPythonClassIdentityCapability> {
};

// Enables template if matches sionna_wrap_caster_enabled, which means that it
// supports all necessary capabilities.
template <typename T>
using enable_if_wrap_caster = std::enable_if_t<sionna_wrap_caster_enabled<T>::value, int>;

}  // namespace artery::sionna::py

namespace nanobind::detail
{

/**
 * @brief Generic caster for wrappers opted-in via nb_wrap_caster_enabled trait.
 */
template <typename T>
struct type_caster<T, artery::sionna::py::enable_if_wrap_caster<T>> {
    NB_TYPE_CASTER(T, const_name<T>())

    // NOTE: Default-constructible is required since caster initializes object to get
    // respective class name.
    static_assert(std::is_default_constructible_v<T>, "Wrap-enabled bridge types must be default-constructible for generic caster");

    bool from_python(handle src, uint8_t /* flags */, cleanup_list* /* list */) noexcept
    {
        if (Value probe; !nanobind::isinstance(src, probe.type())) {
            return false;
        }

        value = Value();
        value.init(borrow<object>(src));
        return true;
    }

    static handle from_cpp(const Value& src, rv_policy /* policy */, cleanup_list* /* cleanup */) noexcept { return src.bound_.inc_ref(); }
};

}  // namespace nanobind::detail
