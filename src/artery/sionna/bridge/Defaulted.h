#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

// Ensure nb::ref is available
#include <artery/sionna/bridge/Casters.h>
#include <artery/sionna/bridge/Fwd.h>

#include <cstring>
#include <type_traits>
#include <utility>
#include <variant>

namespace artery::sionna
{

// Stub for python-provided constants. Caller may provide their own value or this class
// takes care for taking the default value from python.
template <typename T>
class Defaulted
{
public:
    // Value held by both constant value and the one passed instead of it.
    using ValueType = T;
    // Alias for function argument type which may be either plain ValueType of Defaulted stub.
    using Argument = std::variant<T, Defaulted<T>>;

    // Initialize Defaulted class with location of its default value. This implementation
    // supports up to one class deep in resolution.
    constexpr Defaulted(const char* module, const char* name, const char* cls = nullptr) : module_(module), name_(name), cls_(cls) {}

    // Accessors.

    // Name of defaulted variable.
    constexpr const char* name() const { return name_; }

    // Name of module that holds the variable.
    constexpr const char* module() const { return module_; }

    // Name of class that holds the variable. Optional, nullptr means no class
    // exists for current variable.
    constexpr const char* cls() const { return cls_; }

    // Set name for defaulted variable.
    constexpr void setName(const char* name) { name_ = name; }

    // Set module for defaulted variable.
    constexpr void setModule(const char* module) const { module_ = module; }

    // Set class for defaulted variable.
    constexpr void setCls(const char* cls) const { cls_ = cls; }

    // Request constant value from python module.
    virtual T value() const
    {
        nanobind::gil_scoped_acquire gil;

        nanobind::object value;
        // Fetch helpers are not available here, so we use plain accessors.
        if (auto module = nanobind::module_::import_(module_); cls_) {
            value = nanobind::getattr(nanobind::getattr(module, cls_), name_);
        } else {
            value = nanobind::getattr(module, name_);
        }

        return nanobind::cast<T>(value);
    }

    // Resolve value: if user provided their own - use it, otherwise pull from presets.
    static T resolve(Argument variadic)
    {
        return std::visit(
            [](auto&& v) -> T {
                using V = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<V, Defaulted<T>>) {
                    return v.value();
                } else {
                    return v;
                }
            },
            variadic);
    }

private:
    const char* name_ = nullptr;

    // These two may be set later if in compile time their value is unknown.
    mutable const char* cls_ = nullptr;
    mutable const char* module_ = nullptr;
};

// Just like Defaulted, but class and module strings are set at runtime.
template <typename T, typename Resolver>
class DefaultedWithDeferredResolution : public Defaulted<T>
{
public:
    // Initialize DefaultedWithDeferredResolution with defaulted variable name and resolver function,
    // which will provide context to locate the variable should this class need it.
    constexpr DefaultedWithDeferredResolution(const char* name, Resolver resolver) : Defaulted<T>(nullptr, name, nullptr), resolver_(std::move(resolver)) {}

    T value() const override
    {
        using TSimpleContext = const char*;
        using TExtendedContext = std::pair<const char*, const char*>;

        using TReturn = std::invoke_result_t<Resolver&>;

        if constexpr (std::is_same_v<TReturn, TSimpleContext>) {
            this->setModule(resolver_());
        } else if constexpr (std::is_same_v<TReturn, TExtendedContext>) {
            auto [mod, cls] = resolver_();
            this->setModule(mod);
            this->setCls(cls);
        } else {
            static_assert(std::is_same_v<TReturn, void>, "Resolver should return a valid context object");
        }

        return Defaulted<T>::value();
    }

private:
    Resolver resolver_;
};

template <typename>
struct IsDefaulted : std::false_type {
};
template <typename>
struct IsDefaultedArgument : std::false_type {
};

template <typename T>
struct IsDefaulted<Defaulted<T>> : std::true_type {
};
template <typename T, typename R>
struct IsDefaulted<DefaultedWithDeferredResolution<T, R>> : std::true_type {
};

template <typename T>
struct IsDefaultedArgument<std::variant<T, Defaulted<T>>> : std::true_type {
};
template <typename T, typename R>
struct IsDefaultedArgument<std::variant<T, DefaultedWithDeferredResolution<T, R>>> : std::true_type {
};

template <typename V>
inline constexpr bool IsDefaultedV = IsDefaulted<std::decay_t<V>>::value;
template <typename V>
inline constexpr bool IsDefaultedArgumentV = IsDefaultedArgument<std::decay_t<V>>::value;

template <typename>
struct DefaultedArgumentValueType;

template <typename T>
struct DefaultedArgumentValueType<std::variant<T, Defaulted<T>>> {
    using type = T;
};

template <typename T, typename R>
struct DefaultedArgumentValueType<std::variant<T, DefaultedWithDeferredResolution<T, R>>> {
    using type = T;
};

// Key-value args which may have Defaulted types inside.
class Kwargs
{
public:
    // Single item.
    template <typename T>
    using Item = std::pair<const char*, T>;

    // This struct is a hacky way to get value from key.
    struct Key {
        const char* key;

        template <typename T>
        constexpr Item<T> operator=(T v)
        {
            return std::make_pair(key, std::move(v));
        }
    };

    // Return dictionary suitable for regular function/object call, with all args
    // populated if needed.
    template <typename... Args>
    static nanobind::dict toDict(Item<Args>&&... items)
    {
        nanobind::dict d;
        (emplace(d, items.first, std::forward<decltype(items.second)>(items.second)), ...);
        return d;
    }

private:
    template <typename T>
    static void emplace(nanobind::dict& d, const char* key, T&& v)
    {
        using TDecayedValue = std::decay_t<T>;

        if constexpr (IsDefaultedV<TDecayedValue>) {
            // Let python handle defaults.
        } else if constexpr (IsDefaultedArgumentV<TDecayedValue>) {
            using TValueType = typename DefaultedArgumentValueType<TDecayedValue>::type;

            const TDecayedValue& view = v;
            if (const TValueType* value = std::get_if<TValueType>(&view)) {
                d[key] = *value;
            }
        } else {
            d[key] = std::forward<T>(v);
        }
    }
};

namespace literals
{

// Construct the key with convenient literal.
constexpr Kwargs::Key operator""_a(const char* key, std::size_t /* size */)
{
    return Kwargs::Key{.key = key};
}

}  // namespace literals

}  // namespace artery::sionna
