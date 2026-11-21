#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/SionnaBridge.h>
#include <drjit-core/jit.h>
#include <drjit/array.h>
#include <drjit/autodiff.h>
#include <gtest/gtest.h>
#include <mitsuba/core/fwd.h>
#include <mitsuba/mitsuba.h>

#include <filesystem>
#include <initializer_list>
#include <limits>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

namespace artery::sionna::tests
{

// Seed for use within tests. Make sure to make tests deterministic.
constexpr std::size_t seed = 67;

template <typename T>
class Provider;

template <typename T, typename = void>
struct has_provider_next : std::false_type {
};

// Resolves to true if Provider<T>::next() exists and returns T-compatible value.
// clang-format off
    template <typename T>
    struct has_provider_next<
        T,
        std::void_t<
            decltype(std::declval<Provider<T>&>().next())>
        >
        : std::bool_constant<
            std::is_convertible_v<
                  decltype(std::declval<Provider<T>&>().next()),
                  T
            >
        >
    {};
// clang-format on

// Gets provider with Args passed to constructor.
template <typename T, typename... Args>
Provider<T> getProviderFromArgs(Args... args)
{
    using TProvider = Provider<T>;

    // Sanity check to ensure provider is correct.
    if constexpr (!has_provider_next<T>::value) {
        throw wrapRuntimeError("could not build provider for type: %s - next() overload does not exist", typeid(T).name());
    }

    return TProvider(std::forward<Args>(args)...);
}

// Gets provider default-initialized.
template <typename T>
Provider<T> getProvider()
{
    return getProviderFromArgs<T>();
}

class MitsubaLLVMFixture : public ::testing::Test
{
public:
    static void SetUpTestSuite()
    {
        nanobind::gil_scoped_acquire gil;
        auto mi = nanobind::module_::import_("mitsuba");
        if (std::string variant = VariantName::name; variant.find("llvm") == std::string::npos) {
            throw wrapRuntimeError("could not initialize llvm tests: variant is not llvm - %s", VariantName::name);
        }

        mi.attr("set_variant")(VariantName::name);
    }

    // clang-format off
        void SetUp() override {
            // Used to reset backend state.
            #if defined(MI_ENABLE_LLVM)
                jit_init((uint32_t)JitBackend::LLVM);
            #else
                GTEST_SKIP() << "LLVM tests disabled or LLVM backend not enabled.";
            #endif

            if (jit_has_backend(JitBackend::LLVM) == 0) {
                GTEST_FAIL() << "LLVM backend is not available!";
            }
        }
    // clang-format on

    void TearDown() override
    {
        // clang-format off
            #if defined(MI_ENABLE_LLVM)
                jit_shutdown(1);
            #endif
        // clang-format on
    }

public:
    // Epsilon used in float comparisons.
    const double eps_ = std::numeric_limits<float>::epsilon();
};

/*************
 * Factories *
 *************
 */

template <typename TIntegerOrReal>
class RandomNumberProvider
{
public:
    // clang-format off
        using TDistribution = typename std::conditional<
            std::is_integral_v<TIntegerOrReal>,
            std::uniform_int_distribution<TIntegerOrReal>,
            typename std::conditional<
                std::is_floating_point_v<TIntegerOrReal>,
                std::uniform_real_distribution<TIntegerOrReal>,
                void
            >::type
        >::type;
    // clang-format on

    static_assert(!std::is_void_v<TDistribution>, "could not resolve RandomIntegerState distribution type");

    RandomNumberProvider(std::size_t seed, TIntegerOrReal lower, TIntegerOrReal upper) : twister_(seed), dist_(lower, upper) {}

    RandomNumberProvider() : RandomNumberProvider(seed, std::numeric_limits<TIntegerOrReal>::lowest(), std::numeric_limits<TIntegerOrReal>::max()) {}

    TIntegerOrReal next() { return dist_(twister_); }

private:
    TDistribution dist_;
    std::mt19937_64 twister_;
};

// Generates English alphabet chars + chars from inclusive list.
class RandomCharProvider : public RandomNumberProvider<char>
{
public:
    RandomCharProvider(std::size_t seed, std::initializer_list<char> inclusive) :
        RandomNumberProvider<char>(seed, 0, static_cast<char>('z' - 'a' + inclusive.size() - 1))
    {
        for (char c = 'a'; c <= 'z'; ++c) {
            chars_.emplace_back(c);
        }

        for (char c : inclusive) {
            if ('a' <= c && c <= 'z') {
                throw wrapRuntimeError("inclusive parameter should specify non-letter chars");
            }

            chars_.emplace_back(c);
        }
    }

    char next() { return chars_.at(static_cast<std::size_t>(RandomNumberProvider<char>::next())); }

private:
    std::vector<char> chars_;
};

class RandomStringProvider : public RandomCharProvider
{
public:
    static constexpr std::size_t defaultStringSize = 8;
    static constexpr std::initializer_list<char> defaultAdditionalSymbols{'_'};

    RandomStringProvider(std::size_t seed, std::size_t stringSize, std::initializer_list<char> inclusive) :
        RandomCharProvider(seed, inclusive), stringSize_(stringSize)
    {
    }

    RandomStringProvider() : RandomStringProvider(seed, defaultStringSize, defaultAdditionalSymbols) {}

    std::string next()
    {
        std::string generated;
        for (std::size_t i = 0; i < stringSize_; ++i) {
            generated.push_back(RandomCharProvider::next());
        }

        return generated;
    }

private:
    std::size_t stringSize_;
};

/*****************************
 * Register object providers *
 *****************************
 */

template <>
class Provider<int> : public RandomNumberProvider<int>
{
public:
    using RandomNumberProvider<int>::RandomNumberProvider;
};

template <>
class Provider<float> : public RandomNumberProvider<float>
{
public:
    using RandomNumberProvider<float>::RandomNumberProvider;
};

template <>
class Provider<double> : public RandomNumberProvider<double>
{
public:
    using RandomNumberProvider<double>::RandomNumberProvider;
};

template <>
class Provider<std::string> : public RandomStringProvider
{
public:
    using RandomStringProvider::RandomStringProvider;
};

/***************************
 * Bridge Class Generators *
 ***************************
 */

// These classes provide some pseudo-random sample
// clas objects with sane defaults for randomness.

template <>
class Provider<py::RadioMaterial>
{
public:
    Provider() : materialName_(seed, 8, {'_'}), conductivity_(seed, 0.1, 10.0), relativePermittivity_(seed, 1.0, 5.0), thickness_(seed, 0.01, 0.5) {}

    py::RadioMaterial next()
    {
        const std::string materialName = std::string("mat_") + materialName_.next();
        return py::RadioMaterial(materialName, conductivity_.next(), relativePermittivity_.next(), thickness_.next());
    }

private:
    Provider<std::string> materialName_;
    Provider<double> conductivity_;
    Provider<double> relativePermittivity_;
    Provider<double> thickness_;
};

template <>
class Provider<py::SceneObject>
{
public:
    Provider() : objectName_(seed, 8, {'_'}), meshPath_(std::filesystem::current_path() / "sample.ply") {}

    py::SceneObject next()
    {
        if (!std::filesystem::exists(meshPath_)) {
            throw wrapRuntimeError("sample mesh was not found under path: %s", meshPath_.string().c_str());
        }

        const std::string objectName = std::string("obj_") + objectName_.next();
        return py::SceneObject(meshPath_.string(), objectName, material_.next());
    }

private:
    Provider<std::string> objectName_;
    Provider<py::RadioMaterial> material_;
    std::filesystem::path meshPath_;
};

template <>
class Provider<py::SionnaScene>
{
public:
    py::SionnaScene next() { return py::SionnaScene(); }
};

}  // namespace artery::sionna::tests
