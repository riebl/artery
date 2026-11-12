// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/Helpers.h>
#include <drjit-core/jit.h>
#include <drjit/array_router.h>
#include <drjit/jit.h>
#include <gtest/gtest.h>
#include <mitsuba/core/config.h>
#include <mitsuba/core/fwd.h>
#include <mitsuba/core/ray.h>
#include <nanobind/eval.h>

namespace nb = nanobind;

namespace
{

template <typename T>
void validateRuntimeType()
{
    nb::handle pyType = nb::type<T>();
    ASSERT_TRUE(pyType.ptr() != nullptr) << "WARNING: Mitsuba did not register Vector3f. Probably bindings were not loaded?";

    std::string pyTypeName = nb::type_info(pyType).name();
    std::size_t pyTypeSize = nb::type_size(pyType);
    std::size_t pyTypeAlign = nb::type_align(pyType);

    ASSERT_EQ(pyTypeSize, sizeof(T)) << "WARNING: nanobind was registered with type of different size. In this "
                                     << "translation unit the type was compiled to " << sizeof(T) << "bytes, "
                                     << "but instead size should have been " << pyTypeSize << "bytes. Inspect compilation flags.";

    ASSERT_EQ(pyTypeAlign, alignof(T)) << "WARNING: nanobind was registered with type of different alignment. In this "
                                       << "translation unit the type was padded with " << alignof(T) << "bytes, "
                                       << "but instead size should have been " << pyTypeAlign << "bytes. Inspect compilation flags.";

    ASSERT_EQ(pyTypeName, typeid(T).name()) << "Mangled names should match.";
}

}  // namespace

TEST(PythonEmbedTest, CastMitsubaRay)
{
    using Float = float;
    using Spectrum = mitsuba::Color<Float, 3>;

    using namespace mitsuba;
    MI_IMPORT_CORE_TYPES();

    auto mi = nb::module_::import_("mitsuba");
    mi.attr("set_variant")("scalar_rgb");

    validateRuntimeType<Vector3f>();

    auto ray3f = mi.attr("Ray3f");

    Point3f origin(0.1f, 0.2f, 0.3f);
    Vector3f destination(1.0f, 0.0f, 0.0f);

    // These args should be converted into python objects.
    nb::object pyRay = ray3f(nb::cast(origin), nb::cast(destination));

    // Same as above, but the other way around.
    auto cppRay = nb::cast<mitsuba::Ray<mitsuba::CoreAliases<Float>::Point3f, Spectrum>>(pyRay);

    EXPECT_FLOAT_EQ(cppRay.o.x(), 0.1f);
    EXPECT_FLOAT_EQ(cppRay.o.y(), 0.2f);
    EXPECT_FLOAT_EQ(cppRay.o.z(), 0.3f);

    EXPECT_FLOAT_EQ(cppRay.d.x(), 1.0f);
    EXPECT_FLOAT_EQ(cppRay.d.y(), 0.0f);
    EXPECT_FLOAT_EQ(cppRay.d.z(), 0.0f);
}

TEST(PythonEmbedTest, CastDrJitArray)
{
    auto dr = nb::module_::import_("drjit.llvm");
    auto arrayFloat = dr.attr("Float64");

    nb::object pyArr = arrayFloat(1, 2, 3);
    drjit::LLVMArray<double> cppArr = nb::cast<drjit::LLVMArray<double>>(pyArr);

    EXPECT_EQ(cppArr[0], 1);
    EXPECT_EQ(cppArr[1], 2);
    EXPECT_EQ(cppArr[2], 3);
    EXPECT_EQ(drjit::sum(cppArr)[0], 6);
}

TEST(PythonEmbedTest, CastDrJitAdArray)
{
    auto dr = nb::module_::import_("drjit.llvm.ad");
    auto adFloat = dr.attr("Float64");

    nb::object pyAdArr = adFloat(1, 2, 3);
    drjit::DiffArray<JitBackend::LLVM, double> cppAdArr = nb::cast<drjit::DiffArray<JitBackend::LLVM, double>>(pyAdArr);

    EXPECT_EQ(cppAdArr[0], 1);
    EXPECT_EQ(cppAdArr[1], 2);
    EXPECT_EQ(cppAdArr[2], 3);
    EXPECT_EQ(drjit::sum(cppAdArr)[0], 6);
}
