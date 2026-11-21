// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/SionnaBridge.h>
#include <gtest/gtest.h>
#include <nanobind/stl/string.h>

#include <memory>

namespace nb = nanobind;

namespace
{

class DefaultedTest : public testing::Test
{
public:
    void SetUp() override
    {
        nb::gil_scoped_acquire gil;
        try {
            mod_ = std::make_unique<nb::module_>(nb::module_::import_("mod"));
            echo_ = std::make_unique<nb::object>(mod_->attr("echo_kwargs"));
        } catch (const nb::python_error& error) {
            if (error.matches(PyExc_ModuleNotFoundError) || error.matches(PyExc_ImportError)) {
                GTEST_FAIL() << "Failed to import Python test module 'mod'. "
                             << "Run tests via ctest so WORKING_DIRECTORY is set "
                             << "and PYTHONPATH includes the test directory "
                             << "(e.g. `ctest --test-dir build/Release -R test_bridge_module`). "
                             << "If running the binary directly, set "
                             << "`PYTHONPATH=src/artery/sionna/tests/bridge`. "
                             << "Original error: " << error.what();
            }
            GTEST_FAIL() << "Unexpected Python error while importing 'mod': " << error.what();
        }
    }

    static bool dictContains(const nb::dict& d, const std::string& attribute) { return nb::cast<bool>(d.attr("__contains__")(attribute.c_str())); }

    template <typename T>
    T dictFetch(nb::dict d, const std::string& attribute)
    {
        return nb::cast<T>(d[attribute.c_str()]);
    }

    // Handles do not have defaults - so have to allocate them.
    std::unique_ptr<nb::object> echo_;
    std::unique_ptr<nb::module_> mod_;
};

}  // namespace

TEST_F(DefaultedTest, PassingRegularArgsWorks)
{
    using namespace artery::sionna::literals;
    using artery::sionna::Kwargs;

    auto kw = Kwargs::toDict("integer"_a = 1, "double"_a = 2.5, "string"_a = nb::str("hello"));
    nb::dict d = nb::cast<nb::dict>((*echo_)(**kw));

    ASSERT_TRUE(dictContains(d, "integer"));
    ASSERT_TRUE(dictContains(d, "double"));
    ASSERT_TRUE(dictContains(d, "string"));

    EXPECT_EQ(dictFetch<int>(d, "integer"), 1);
    EXPECT_DOUBLE_EQ(dictFetch<double>(d, "double"), 2.5);
    EXPECT_EQ(dictFetch<std::string>(d, "string"), "hello");
}

TEST_F(DefaultedTest, ResolveWorks)
{
    using D = artery::sionna::Defaulted<int>;

    D global{"mod", "GLOBAL_CONST"};
    D belongsToCls{"mod", "CONST", "Constants"};

    D::Argument arg = 7;
    EXPECT_EQ(D::resolve(arg), 7);

    D::Argument defaulted = global;
    EXPECT_EQ(D::resolve(defaulted), 1);

    defaulted = belongsToCls;
    EXPECT_EQ(D::resolve(defaulted), 2);
}

TEST_F(DefaultedTest, DefaultedArgsAreNotPassed)
{
    using namespace artery::sionna::literals;
    using artery::sionna::Kwargs;
    using D = artery::sionna::Defaulted<int>;

    D defaulted{"mod", "GLOBAL_CONST"};

    {
        // Make sure kwargs() resolves Defaulted type - if passed
        // to caster, this will break nanobind core library (nanobind-drjit).
        auto kw = Kwargs::toDict("x"_a = defaulted, "y"_a = 42);

        nb::dict d = nb::cast<nb::dict>((*echo_)(**kw));

        // Default arguments are left out intentionally.
        ASSERT_FALSE(dictContains(d, "x"));
        ASSERT_TRUE(dictContains(d, "y"));

        EXPECT_EQ(dictFetch<int>(d, "y"), 42);
    }

    {
        D::Argument x = 99;

        auto kw = Kwargs::toDict("x"_a = x);
        nb::dict d = nb::cast<nb::dict>((*echo_)(**kw));

        ASSERT_TRUE(dictContains(d, "x"));
        EXPECT_EQ(dictFetch<int>(d, "x"), 99);
    }
}
