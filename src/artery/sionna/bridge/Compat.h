#pragma once

#include <artery/sionna/bridge/Fwd.h>
#include <drjit-core/traits.h>
#include <drjit/array_traits.h>
#include <drjit/jit.h>
#include <mitsuba/core/bbox.h>
#include <mitsuba/core/vector.h>

#include <type_traits>

/**
 * @file Helpers for converting Mitsuba datatypes into various user-end frinedly
 * types.
 */

namespace artery::sionna
{

// Resolves to false boolean value for any template.
template <typename...>
constexpr static bool always_false_v = false;

// Converter implementation.
template <typename OutValue, typename InValue, typename STUBBER = void>
struct impl;

// Converter resolution - will resolve to failed cast if suitable caster is not visible.
template <typename OutValue, typename InValue>
static OutValue convert(InValue value)
{
    return impl<OutValue, InValue>::convert(value);
}

template <typename OutValue, typename InValue, typename STUBBER = void>
struct CompatImpl {
    static OutValue convert(InValue /* value */)
    {
        static_assert(always_false_v<OutValue, InValue>, "convert: no conversion available for requested input/output types");
    }
};

template <typename OutValue, typename InValue, typename STUBBER>
struct impl : CompatImpl<OutValue, InValue, STUBBER> {
};

// Ensure passed value is scalar. If passing scalar type, it gets returned back to caller.
// Also performs static cast if scalar types mismatch.
template <typename ReturnT = double, typename ValueT>
static ReturnT toScalar(const ValueT& value)
{
    if constexpr (drjit::is_array_v<ValueT>) {
        return static_cast<ReturnT>(drjit::slice(drjit::detach(value), 0));
    } else {
        return static_cast<ReturnT>(value);
    }
}

// Ensure returned value is compatible with Float template parameter. Generally, you shall call just call Float(),
// but the thing is - it may not always work, so I implemented this generic converter.
template <typename Float, typename ScalarT>
static Float fromScalar(const ScalarT& value)
{
    if constexpr (std::is_floating_point_v<Float>) {
        if constexpr (std::is_same_v<Float, std::decay_t<ScalarT>>) {
            return value;
        }
        return static_cast<Float>(value);
    } else if constexpr (drjit::is_llvm_v<Float> || drjit::is_cuda_v<Float>) {
        return Float(value);
    }
}

// For LLVM non-AD arrays, select their AD counterpart. Otherwise return T.
template <typename T>
struct MaybeDiff {
    using type = T;
};

template <typename T>
    requires(drjit::is_llvm_v<T> && !drjit::is_diff_v<T>)
struct MaybeDiff<T> {
    using type = drjit::LLVMDiffArray<drjit::scalar_t<T>>;
};

template <typename T>
using maybe_diff_t = typename MaybeDiff<T>::type;

}  // namespace artery::sionna
