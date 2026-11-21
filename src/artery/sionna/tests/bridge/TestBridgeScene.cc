#include <artery/sionna/bridge/SionnaBridge.h>
#include <artery/sionna/tests/bridge/MitsubaFixture.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <variant>

namespace artery::sionna::tests
{

TEST_F(MitsubaLLVMFixture, EditAddsGetsAndRemovesSceneObject)
{
    auto scene = getProvider<py::SionnaScene>().next();

    auto missing = scene.get("missing");
    EXPECT_TRUE(std::holds_alternative<std::monostate>(missing));

    auto addedObject = getProvider<py::SceneObject>().next();
    scene.edit({addedObject}, {});

    auto objects = scene.sceneObjects();
    ASSERT_EQ(objects.size(), 1u);
    const auto objectName = objects.begin()->first;

    auto objectFromMap = objects.at(objectName);
    const auto position = mitsuba::Resolve::Point3f(1.0f, 2.0f, 3.0f);
    objectFromMap.setPosition(position);
    auto objectPosition = objectFromMap.position();
    EXPECT_NEAR(artery::sionna::toScalar<double>(objectPosition.x()), 1.0, eps_);
    EXPECT_NEAR(artery::sionna::toScalar<double>(objectPosition.y()), 2.0, eps_);
    EXPECT_NEAR(artery::sionna::toScalar<double>(objectPosition.z()), 3.0, eps_);

    auto fetched = scene.get(objectName);
    ASSERT_TRUE(std::holds_alternative<py::SceneObject>(fetched));

    auto fetchedObject = std::get<py::SceneObject>(fetched);
    auto fetchedPosition = fetchedObject.position();
    EXPECT_NEAR(artery::sionna::toScalar<double>(fetchedPosition.x()), 1.0, eps_);
    EXPECT_NEAR(artery::sionna::toScalar<double>(fetchedPosition.y()), 2.0, eps_);
    EXPECT_NEAR(artery::sionna::toScalar<double>(fetchedPosition.z()), 3.0, eps_);

    auto materials = scene.radioMaterials();
    auto expectedMaterial = addedObject.material();
    ASSERT_TRUE(materials.contains(expectedMaterial.materialName()));
    EXPECT_EQ(materials.at(expectedMaterial.materialName()).materialName(), expectedMaterial.materialName());

    scene.edit({}, {objectName});

    EXPECT_TRUE(scene.sceneObjects().empty());
    EXPECT_TRUE(std::holds_alternative<std::monostate>(scene.get(objectName)));
}

}  // namespace artery::sionna::tests
