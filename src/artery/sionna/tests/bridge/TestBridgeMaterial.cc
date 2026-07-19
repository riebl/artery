#include <artery/sionna/bridge/SionnaBridge.h>
#include <artery/sionna/tests/bridge/MitsubaFixture.h>
#include <gtest/gtest.h>

namespace artery::sionna::tests
{

TEST_F(MitsubaLLVMFixture, ConstructAndAccessProperties)
{
    auto material = getProvider<py::RadioMaterial>().next();

    EXPECT_NO_FATAL_FAILURE(material.materialName());

    constexpr double newThickness = 0.1;
    constexpr double newConductivity = 0.3;
    constexpr double newRelativePermittivity = 1.5;

    material.setThickness(fromScalar<mitsuba::Resolve::Float>(newThickness));
    material.setConductivity(fromScalar<mitsuba::Resolve::Float>(newConductivity));
    material.setRelativePermittivity(fromScalar<mitsuba::Resolve::Float>(newRelativePermittivity));

    const double thickness = toScalar<double>(material.thickness());
    const double conductivity = toScalar<double>(material.conductivity());
    const double permittivity = toScalar<double>(material.relativePermittivity());

    EXPECT_NEAR(thickness, newThickness, eps_);
    EXPECT_NEAR(conductivity, newConductivity, eps_);
    EXPECT_NEAR(permittivity, newRelativePermittivity, eps_);
}

TEST_F(MitsubaLLVMFixture, ColorRoundTrip)
{
    auto material = getProvider<py::RadioMaterial>().next();

    py::RadioMaterial::TColor newColor{0.1, 0.2, 0.3};
    material.setColor(newColor);

    auto color = material.color();
    const double r = toScalar<double>(std::get<0>(color));
    const double g = toScalar<double>(std::get<1>(color));
    const double b = toScalar<double>(std::get<2>(color));

    EXPECT_NEAR(r, std::get<0>(newColor), eps_);
    EXPECT_NEAR(g, std::get<1>(newColor), eps_);
    EXPECT_NEAR(b, std::get<2>(newColor), eps_);
}

}  // namespace artery::sionna::tests
