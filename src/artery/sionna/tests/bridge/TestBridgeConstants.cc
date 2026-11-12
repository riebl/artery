#include <artery/sionna/bridge/Compat.h>
#include <artery/sionna/bridge/bindings/Constants.h>
#include <artery/sionna/tests/bridge/MitsubaFixture.h>
#include <gtest/gtest.h>

#include <cmath>
#include <memory>

using namespace artery::sionna;

namespace
{

class ConstantsLLVMFixture : public artery::sionna::tests::MitsubaLLVMFixture
{
public:
    void SetUp() override
    {
        MitsubaLLVMFixture::SetUp();
        nanobind::gil_scoped_acquire gil;
        module_ = std::make_unique<nanobind::module_>(nanobind::module_::import_("sionna.rt.constants"));
    }

    std::unique_ptr<nanobind::module_> module_;
};

}  // namespace

TEST_F(ConstantsLLVMFixture, DefaultThicknessViaBridgeResolves)
{
    const mitsuba::Resolve::Float64 value = py::Constants::defaultThickness.value();
    EXPECT_TRUE(std::isfinite(toScalar<double>(value)));
}

TEST_F(ConstantsLLVMFixture, InteractionTypesViaBridgeResolve)
{
    using TIntersection = py::IntersectionTypes;

    // clang-format off
    for (const auto& var : {
             TIntersection::none,
             TIntersection::specular,
             TIntersection::diffuse,
             TIntersection::refraction,
             TIntersection::diffraction
        }
    ) {
        EXPECT_NO_FATAL_FAILURE(toScalar<int>(var.value()));
    }
    // clang-format on
}
