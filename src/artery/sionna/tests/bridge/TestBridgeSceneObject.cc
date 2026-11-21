#include <artery/sionna/bridge/SionnaBridge.h>
#include <artery/sionna/tests/bridge/MitsubaFixture.h>
#include <gtest/gtest.h>
#include <mitsuba/core/object.h>
#include <mitsuba/core/plugin.h>
#include <mitsuba/core/properties.h>

#define protected public
#include <mitsuba/render/mesh.h>
#undef protected

#include <filesystem>

namespace artery::sionna::tests
{

TEST_F(MitsubaLLVMFixture, TestSceneObjectInitializesFromMeshPath)
{
    nanobind::gil_scoped_acquire gil;

    py::RadioMaterial material("test_mat", /* conductivity = */ 2.5, /* relativePermittivity = */ 3.5, /* thickness = */ 0.2);

    auto sample = std::filesystem::current_path() / "sample.ply";
    if (!std::filesystem::exists(sample)) {
        GTEST_FAIL() << "sample mesh was not found under path: " << sample.string();
    }

    py::SceneObject sceneObject(sample.string(), "some_obj", material);
    EXPECT_EQ(sceneObject.name(), "some_obj");

    // Technically, we can initialize LLVM arrays from scalar values,
    // which means that assertions like EXPECT_EQUALS(<some_llvm_array>, <some_float_value>)
    // should work, but I prefer to use toScalar just be sure that test failing will print
    // something meaningful.

    auto pos = sceneObject.position();
    EXPECT_NEAR(toScalar<double>(pos.x()), 0.5, eps_);
    EXPECT_NEAR(toScalar<double>(pos.y()), 0.5, eps_);
    EXPECT_NEAR(toScalar<double>(pos.z()), 0.0, eps_);

    auto orientation = sceneObject.orientation();
    EXPECT_NEAR(toScalar<double>(orientation.x()), 0.0, eps_);
    EXPECT_NEAR(toScalar<double>(orientation.y()), 0.0, eps_);
    EXPECT_NEAR(toScalar<double>(orientation.z()), 0.0, eps_);

    auto mat = sceneObject.material();
    EXPECT_EQ(mat.materialName(), "test_mat");
}

TEST_F(MitsubaLLVMFixture, InitFromMitsubaMesh)
{
    nanobind::gil_scoped_acquire gil;

    auto sample = std::filesystem::current_path() / "sample.ply";
    if (!std::filesystem::exists(sample)) {
        GTEST_FAIL() << "current working directory should be set to CMAKE_CURRENT_SOURCE_DIR so test may resolve all data files!";
    }

    mitsuba::Properties props("ply");
    props.set("filename", sample.string());
    auto mesh = mitsuba::PluginManager::instance()->create_object<mitsuba::Resolve::Mesh>(props);
    py::RadioMaterial material("test_mat", /* conductivity = */ 2.5, /* relativePermittivity = */ 3.5, /* thickness = */ 0.2);

    py::SceneObject sceneObject(mesh, "mesh_obj", material);
    EXPECT_EQ(sceneObject.name(), "mesh_obj");

    ASSERT_NE(sceneObject.mesh().get(), nullptr);
    EXPECT_EQ(sceneObject.mesh().get(), mesh.get());

    auto pos = sceneObject.position();
    EXPECT_NEAR(artery::sionna::toScalar<double>(pos.x()), 0.5, eps_);
    EXPECT_NEAR(artery::sionna::toScalar<double>(pos.y()), 0.5, eps_);
    EXPECT_NEAR(artery::sionna::toScalar<double>(pos.z()), 0.0, eps_);

    auto orient = sceneObject.orientation();
    EXPECT_NEAR(artery::sionna::toScalar<double>(orient.x()), 0.0, eps_);
    EXPECT_NEAR(artery::sionna::toScalar<double>(orient.y()), 0.0, eps_);
    EXPECT_NEAR(artery::sionna::toScalar<double>(orient.z()), 0.0, eps_);
}

TEST_F(MitsubaLLVMFixture, MutateProperties)
{
    nanobind::gil_scoped_acquire gil;

    py::RadioMaterial initialMaterial("initial_mat", /* conductivity = */ 2.5, /* relativePermittivity = */ 3.5, /* thickness = */ 0.2);
    py::RadioMaterial updatedMaterial("updated_mat", /* conductivity = */ 5.0, /* relativePermittivity = */ 6.0, /* thickness = */ 0.4);

    auto sample = std::filesystem::current_path() / "sample.ply";
    if (!std::filesystem::exists(sample)) {
        GTEST_FAIL() << "current working directory should be set to CMAKE_CURRENT_SOURCE_DIR so test may resolve all data files!";
    }

    py::SceneObject sceneObject(sample.string(), "some_obj", initialMaterial);
    sceneObject.setMaterial(updatedMaterial);

    EXPECT_EQ(sceneObject.name(), "some_obj");
    auto actualMaterial = sceneObject.material();
    EXPECT_EQ(actualMaterial.materialName(), "updated_mat");
    EXPECT_NEAR(artery::sionna::toScalar<double>(actualMaterial.conductivity()), 5.0, eps_);
    EXPECT_NEAR(artery::sionna::toScalar<double>(actualMaterial.relativePermittivity()), 6.0, eps_);
    EXPECT_NEAR(artery::sionna::toScalar<double>(actualMaterial.thickness()), 0.4, eps_);
}

}  // namespace artery::sionna::tests
