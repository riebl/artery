#pragma once

// FIXME: Remove this workaround once INET no longer exposes NaN as a macro
#ifdef NaN
#undef NaN
#endif

// Mitsuba must establish its core and render aliases before Dr.Jit traits are loaded.
// clang-format off
#include <mitsuba/core/fwd.h>
#include <mitsuba/render/fwd.h>
#include <mitsuba/core/parser.h>
#include <drjit/array_traits.h>
// clang-format on

namespace mitsuba
{

// This class is used as an accessor for mitsuba templates types, with
// Float and Spectrum aliases resolving to effective base types used.
class Resolve
{
public:
    // clang-format seems to take down spaces before macros, which makes them
    // stand out from class even though they should not.

    // clang-format off
        #if defined(SIONNA_BRIDGE_RESOLVE_FLOAT)
            using Float = SIONNA_BRIDGE_RESOLVE_FLOAT;
        #else
            using Float = float;
        #endif

        #if defined(SIONNA_BRIDGE_RESOLVE_SPECTRUM)
            using Spectrum = SIONNA_BRIDGE_RESOLVE_SPECTRUM;
        #else
            using Spectrum = Color<Float, 3>;
        #endif

        #if not defined(MI_ENABLE_LLVM)
            #define MI_ENABLE_LLVM 0
        #endif
    // clang-format on

    static_assert(!drjit::is_llvm_v<Float> || MI_ENABLE_LLVM, "Make sure MI_ENABLE_LLVM is defined when LLVM types are used");

    // We may add support for CUDA later.

    // MI_IMPORT_RENDER_BASIC_TYPES already imports core types.
    MI_IMPORT_TYPES()
    MI_IMPORT_OBJECT_TYPES()
};

}  // namespace mitsuba

// SB API partially uses nanobind objects. Because of this, where applicable,
// we should comply with nanobind symbols visibility.

#if defined(__GNUC__) && !defined(_WIN32)
#define SIONNA_BRIDGE_API __attribute__((visibility("default")))
#define SIONNA_BRIDGE_INTERNAL __attribute__((visibility("hidden")))
#else
#define SIONNA_BRIDGE_API
#define SIONNA_BRIDGE_INTERNAL
#endif

namespace artery::sionna
{

// Convenient alias.
using mi = mitsuba::Resolve;

// Static access to current variant name.
using VariantName = mitsuba::detail::variant<mitsuba::Resolve::Float, mitsuba::Resolve::Spectrum>;
}  // namespace artery::sionna
