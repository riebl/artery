#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/Defaulted.h>
#include <drjit/array_traits.h>

#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>

#if defined(SIONNA_BRIDGE_COMPILATION_MODE_BASIC)
#include <stdexcept>
#elif defined(SIONNA_BRIDGE_COMPILATION_MODE_INET)
#include <omnetpp/cexception.h>
#endif

namespace artery::sionna
{

// Formats with std::snprintf and returns std::string.
template <typename... Args>
std::string format(const std::string& fmt, Args... args)
{
    if constexpr (sizeof...(args) == 0) {
        return fmt;
    } else if (const int size = std::snprintf(nullptr, 0, fmt.c_str(), args...) + 1; size > 0) {
        auto buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, fmt.c_str(), args...);
        return buf.get();
    }

    return "failed to format string: std::snprintf returned invalid size";
}

// Common exception factory.
template <typename... Args>
auto wrapRuntimeError(const std::string& fmt, Args... args)
{
    // clang-format off
        #if defined(SIONNA_BRIDGE_COMPILATION_MODE_BASIC)
            return std::runtime_error(format(fmt, std::forward<Args>(args)...));
        #elif defined(SIONNA_BRIDGE_COMPILATION_MODE_INET)
            return omnetpp::cRuntimeError(fmt.c_str(), args...);
        #endif
    // clang-format on
}

// Manage embedded python interpreter with minimal config in RAII style, setting
// essential paths to home, exec_dir, and package discovery paths.
class ScopedInterpreter
{
public:
    using path = std::filesystem::path;

    /**
     * @brief Initialize interpreter, considering \ref root as root directory.
     * @param root prefix for python installation.
     */
    ScopedInterpreter(const std::filesystem::path& root)
    {
        PyConfig_InitPythonConfig(&config_);
        if (PyStatus status = initialize(root); PyStatus_IsError(status)) {
            throw wrapRuntimeError("sionna: failed to initialize interpreter config, error: %s", status.err_msg);
        }

        if (PyStatus status = Py_InitializeFromConfig(&config_); PyStatus_IsError(status)) {
            throw wrapRuntimeError("sionna: failed to start interpreter, reported error: %s", status.err_msg);
        }
    }

    ScopedInterpreter(const ScopedInterpreter&) = delete;
    ScopedInterpreter(ScopedInterpreter&&) = delete;

    ScopedInterpreter& operator=(const ScopedInterpreter&) = delete;
    ScopedInterpreter& operator=(ScopedInterpreter&&) = delete;

    virtual ~ScopedInterpreter()
    {
        PyConfig_Clear(&config_);
        Py_Finalize();
    }

    // You may override the functions below to customize how this class
    // manages paths to various directories.
    virtual path program(const path& root) const { return root / "bin" / "python3"; }
    virtual path pythonpath(const path& root) const { return root.parent_path(); }

protected:
    /**
     * @brief Initialize configuration.
     * Update @ref config_ with options you want. This method is invoked before python
     * is initialized, so take care: CPython API usage is heavily limited.
     */
    virtual PyStatus initialize(const path& root)
    {
        // Program name is usually enough - the rest is discovered by python automatically.
        std::wstring program = this->program(root).wstring();
        PyStatus status = PyConfig_SetString(&config_, &config_.program_name, program.c_str());
        if (PyStatus_IsError(status) != 0) {
            return status;
        }

        // clang-format off
            // Add project root to PYTHONPATH when running in-place.
            #if not defined(NDEBUG)
                std::wstring python = this->pythonpath(root).wstring();
                status = PyConfig_SetString(&config_, &config_.pythonpath_env, python.c_str());
            #endif
        // clang-format on

        return status;
    }

private:
    PyConfig config_;
};

namespace detail
{

inline nanobind::object fetch(const nanobind::object& obj, const std::string& attribute)
{
    return obj.attr(attribute.c_str());
}

template <typename T>
T cast(const nanobind::object& attr, const std::string& name)
{
    try {
        return nanobind::cast<T>(attr);
    } catch (const std::bad_cast& err) {
        const char* pyType = Py_TYPE(attr.ptr())->tp_name;
        const char* cppType = typeid(T).name();
        throw wrapRuntimeError("sionna: failed to cast an atrribute \"%s\" of type \"%s\" to host type \"%s\"", name.c_str(), pyType, cppType);
    }
}

template <typename T, std::enable_if_t<std::is_base_of_v<nanobind::handle, std::decay_t<T>>, int> = 0>
T access(const nanobind::object& obj, const std::string& attribute)
{
    return nanobind::borrow<T>(fetch(obj, attribute));
}

template <typename T, std::enable_if_t<drjit::is_array_v<T>, int> = 0>
T access(const nanobind::object& obj, const std::string& attribute)
{
    using TScalar = drjit::scalar_t<T>;
    nanobind::object attr = fetch(obj, attribute);

    // Try scalar conversions first to keep scalar and array values compatible.
    if (nanobind::isinstance<nanobind::int_>(attr)) {
        return T(static_cast<TScalar>(nanobind::cast<long long>(attr)));
    }

    if (nanobind::isinstance<nanobind::float_>(attr)) {
        return T(static_cast<TScalar>(nanobind::cast<double>(attr)));
    }

    return detail::cast<T>(attr, attribute);
}

template <typename T, std::enable_if_t<!std::is_base_of_v<nanobind::handle, std::decay_t<T>> && !drjit::is_array_v<T>, int> = 0>
T access(const nanobind::object& obj, const std::string& attribute)
{
    return detail::cast<T>(fetch(obj, attribute), attribute);
}

}  // namespace detail

// Access an attribute on object with a cast.
template <typename T>
T access(const nanobind::object& obj, const std::string& attribute)
{
    nanobind::gil_scoped_acquire gil;
    return detail::access<T>(obj, attribute);
}

// Set an attribute on a Sionna Python object, with automatic type wrapping.
template <typename T>
void set(const nanobind::object& obj, const std::string& attribute, T value)
{
    nanobind::gil_scoped_acquire gil;

    try {
        if constexpr (std::is_base_of_v<nanobind::handle, std::decay_t<T>>) {
            nanobind::setattr(obj, attribute.c_str(), value);
        } else {
            nanobind::setattr(obj, attribute.c_str(), nanobind::cast(std::move(value)));
        }
    } catch (const nanobind::python_error& error) {
        throw wrapRuntimeError("sionna: failed to set property \"%s\" of object at %p: %s", attribute.c_str(), obj.ptr(), error.what());
    }
}

// Call an object.
template <typename ReturnT = nanobind::object, typename... Args>
ReturnT callObject(const nanobind::object& obj, Args&&... args)
{
    nanobind::gil_scoped_acquire gil;

    nanobind::dict kw = Kwargs::toDict(std::forward<Args>(args)...);
    try {
        return obj(**kw);
    } catch (const nanobind::python_error& error) {
        throw wrapRuntimeError("sionna: failed to call an object at %p: %s", obj.ptr(), error.what());
    }
}

// Call an a specified attribute on an object.
template <typename ReturnT = nanobind::object, typename... Args>
ReturnT call(const nanobind::object& obj, const std::string& method, Args&&... args)
{
    nanobind::gil_scoped_acquire gil;

    return callObject<ReturnT>(sionna::access<nanobind::object>(obj, method), std::forward<Args>(args)...);
}

}  // namespace artery::sionna
