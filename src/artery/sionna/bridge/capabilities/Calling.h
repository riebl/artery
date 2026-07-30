#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/Defaulted.h>
#include <artery/sionna/bridge/capabilities/Core.h>

#include <utility>

namespace artery::sionna::py
{

/**
 * @brief Capability to call arbitrary methods in python.
 */
class SIONNA_BRIDGE_API CallAnyCapability : public virtual IPythonCapability
{
public:
    /*
     * @brief call a callable attribute on target. This method runs type convertions
     * as needed and supported.
     */
    template <typename ReturnType = nanobind::object, typename... Args>
    ReturnType callAny(nanobind::handle target, const char* name, Args&&... args) const
    {
        nanobind::gil_scoped_acquire gil;
        nanobind::dict kw = Kwargs::toDict(std::forward<Args>(args)...);
        nanobind::object callable = nanobind::getattr(target, name);
        return nanobind::cast<ReturnType>(callable(**kw));
    }
};

/**
 * @brief Capability to initialize an object.
 */
class SIONNA_BRIDGE_API InitPythonClassCapability : public BasePythonFetchCapability, public CallAnyCapability, public ExportBoundObjectCapability
{
public:
    using ExportBoundObjectCapability::init;

    /**
     * @brief Call a type object to initialize python object.
     */
    template <typename... Args>
    void init(Args&&... args)
    {
        nanobind::gil_scoped_acquire gil;
        nanobind::dict kw = Kwargs::toDict(std::forward<Args>(args)...);
        bound_ = type()(**kw);
    }
};

}  // namespace artery::sionna::py
