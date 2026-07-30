#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/Fwd.h>

#include <type_traits>
#include <utility>

namespace artery::sionna::py
{

// Trait that resolves to true if types supports all requested capabilities.
template <typename T, typename... Caps>
struct supports_capabilities : std::bool_constant<(std::is_base_of_v<Caps, T> && ...)> {
};

// Boolean variant for supports_capabilities.
template <typename T, typename... Caps>
constexpr bool supports_capabilities_v = supports_capabilities<T, Caps...>::value;

// Base for all capabilities.
class SIONNA_BRIDGE_API IPythonCapability
{
public:
    virtual ~IPythonCapability() = default;
};

/**
 * @brief Capability to identify a module. Override with qualified path to
 * module, that gets implemented.
 */
class SIONNA_BRIDGE_API IPythonModuleIdentityCapability : public virtual IPythonCapability
{
public:
    ~IPythonModuleIdentityCapability() override = default;

    /*
     * @brief return module name. Must be known at compile time.
     * @returns fully quilified module name (path) in a string like "path.to.module".
     */
    constexpr virtual const char* moduleName() const = 0;
};

/**
 * @brief Capability to identify a class. Inherits from module identity capability,
 * since every class belongs to a module.
 */
class SIONNA_BRIDGE_API IPythonClassIdentityCapability : public virtual IPythonModuleIdentityCapability
{
public:
    ~IPythonClassIdentityCapability() override = default;

    /*
     * @brief return class name only. Must be known at compile time.
     * @returns just the class name.
     */
    constexpr virtual const char* className() const = 0;
};

/**********************
 * Basic capabilities *
 **********************
 */

/**
 * @brief Basic stateless python module import capability.
 */
class SIONNA_BRIDGE_API BasePythonImportCapability : public virtual IPythonModuleIdentityCapability
{
public:
    /**
     * @brief Stateless module import.
     */
    virtual nanobind::module_ module() const
    {
        nanobind::gil_scoped_acquire gil;
        return nanobind::module_::import_(moduleName());
    }
};

/**
 * @brief Cached access to a module. Stores module upon first import, which
 * allows effective multiple access to it.
 */
class SIONNA_BRIDGE_API CachedImportCapability : public BasePythonImportCapability
{
public:
    /**
     * @brief Stateful (cached) module import.
     */
    nanobind::module_ module() const override
    {
        if (!module_.is_valid()) {
            nanobind::gil_scoped_acquire gil;
            module_ = BasePythonImportCapability::module();
        }
        return nanobind::borrow<nanobind::module_>(module_.ptr());
    }

protected:
    mutable nanobind::object module_;
};

/**
 * @brief Basic class (type) stateless object access.
 */
class SIONNA_BRIDGE_API BasePythonFetchCapability : public virtual IPythonClassIdentityCapability, public BasePythonImportCapability
{
public:
    /**
     * @brief Get type object from module (stateless).
     */
    virtual nanobind::object type() const
    {
        nanobind::gil_scoped_acquire gil;
        return nanobind::getattr(module(), className());
    }
};

/**
 * @brief Class (type) stateful object access.
 */
class SIONNA_BRIDGE_API CachedFetchCapability : public BasePythonFetchCapability
{
public:
    /**
     * @brief Get type object from module (stateful).
     */
    nanobind::object type() const override
    {
        if (!type_.is_valid()) {
            nanobind::gil_scoped_acquire gil;
            type_ = BasePythonFetchCapability::type();
        }
        return type_;
    }

protected:
    mutable nanobind::object type_;
};

/**
 * @brief Capability that holds bound python object.
 */
class SIONNA_BRIDGE_API BoundPythonClassCapability : public IPythonCapability
{
protected:
    nanobind::object bound_;

    template <typename T, typename>
    friend struct nanobind::detail::type_caster;
};

/**
 * @brief Capability to wrap existing object.
 */
class SIONNA_BRIDGE_API WrapPythonClassCapability : public BoundPythonClassCapability
{
public:
    void init(nanobind::object obj) { bound_ = std::move(obj); }
};

/**
 * @brief Capability to expose the underlying bound Python object.
 */
class SIONNA_BRIDGE_API ExportBoundObjectCapability : public WrapPythonClassCapability
{
public:
    nanobind::object object() const { return this->bound_; }
};

}  // namespace artery::sionna::py
