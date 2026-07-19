// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/Fwd.h>
#include <gtest/gtest.h>
#include <mitsuba/core/config.h>
#include <mitsuba/core/fwd.h>
#include <mitsuba/core/ray.h>

namespace nb = nanobind;

namespace
{

template <typename T>
void checkCoreTypeLayout(const std::string& label)
{
    nb::handle pyType;

    pyType = nb::type<T>();
    ASSERT_TRUE(pyType.ptr() != nullptr) << "WARNING: Mitsuba did not register " << label << " type. Probably bindings were not loaded?";

    std::size_t pySize = nb::type_size(pyType);
    std::size_t pyAlign = nb::type_align(pyType);
    std::string pyName = nb::type_info(pyType).name();

    const std::size_t localSize = sizeof(T);
    const std::size_t localAlign = alignof(T);
    std::string localName = typeid(T).name();

    EXPECT_EQ(pySize, localSize) << "Size mismatch for '" << label << "': "
                                 << "nanobind registered " << pySize << " bytes, local has " << localSize << " bytes.";

    EXPECT_EQ(pyAlign, localAlign) << "Alignment mismatch for '" << label << "': "
                                   << "nanobind registered " << pyAlign << " bytes, local has " << localAlign << " bytes.";

    // I don't think this one ever fails :)
    EXPECT_EQ(pyName, localName) << "Mangled name mismatch for '" << label << "': "
                                 << "nanobind registered " << pyName << " local has " << localName;
}

}  // namespace

TEST(PythonEmbedTest, TestTypesLaidOutProperly)
{
    using Float = float;
    using Spectrum = mitsuba::Color<Float, 3>;

    namespace dr = drjit;

    MI_IMPORT_CORE_TYPES()

    ASSERT_EQ(MI_DEFAULT_VARIANT, "scalar_rgb") << "Default variant is not scalar_rgb, which means this test will ineventby fail. "
                                                << "Please recompile with different Mitsuba presets.";

    auto mi = nb::module_::import_("mitsuba");
    mi.attr("set_variant")(MI_DEFAULT_VARIANT);

    checkCoreTypeLayout<Vector1i>("Vector1i");
    checkCoreTypeLayout<Vector2i>("Vector2i");
    checkCoreTypeLayout<Vector3i>("Vector3i");
    checkCoreTypeLayout<Vector4i>("Vector4i");

    checkCoreTypeLayout<Vector1u>("Vector1u");
    checkCoreTypeLayout<Vector2u>("Vector2u");
    checkCoreTypeLayout<Vector3u>("Vector3u");
    checkCoreTypeLayout<Vector4u>("Vector4u");

    checkCoreTypeLayout<Vector1f>("Vector1f");
    checkCoreTypeLayout<Vector2f>("Vector2f");
    checkCoreTypeLayout<Vector3f>("Vector3f");
    checkCoreTypeLayout<Vector4f>("Vector4f");

    checkCoreTypeLayout<Vector1d>("Vector1d");
    checkCoreTypeLayout<Vector2d>("Vector2d");
    checkCoreTypeLayout<Vector3d>("Vector3d");
    checkCoreTypeLayout<Vector4d>("Vector4d");

    checkCoreTypeLayout<Point1i>("Point1i");
    checkCoreTypeLayout<Point2i>("Point2i");
    checkCoreTypeLayout<Point3i>("Point3i");
    checkCoreTypeLayout<Point4i>("Point4i");

    checkCoreTypeLayout<Point1u>("Point1u");
    checkCoreTypeLayout<Point2u>("Point2u");
    checkCoreTypeLayout<Point3u>("Point3u");
    checkCoreTypeLayout<Point4u>("Point4u");

    checkCoreTypeLayout<Point1f>("Point1f");
    checkCoreTypeLayout<Point2f>("Point2f");
    checkCoreTypeLayout<Point3f>("Point3f");
    checkCoreTypeLayout<Point4f>("Point4f");

    checkCoreTypeLayout<Point1d>("Point1d");
    checkCoreTypeLayout<Point2d>("Point2d");
    checkCoreTypeLayout<Point3d>("Point3d");
    checkCoreTypeLayout<Point4d>("Point4d");

    checkCoreTypeLayout<Normal3f>("Normal3f");
    checkCoreTypeLayout<Normal3d>("Normal3d");

    checkCoreTypeLayout<Matrix2f>("Matrix2f");
    checkCoreTypeLayout<Matrix2d>("Matrix2d");
    checkCoreTypeLayout<Matrix3f>("Matrix3f");
    checkCoreTypeLayout<Matrix3d>("Matrix3d");
    checkCoreTypeLayout<Matrix4f>("Matrix4f");
    checkCoreTypeLayout<Matrix4d>("Matrix4d");

    checkCoreTypeLayout<Color1f>("Color1f");
    checkCoreTypeLayout<Color3f>("Color3f");
    checkCoreTypeLayout<Color1d>("Color1d");
    checkCoreTypeLayout<Color3d>("Color3d");
}
