// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/tests/bridge/MitsubaFixture.h>
#include <gtest/gtest.h>

#include <string>
#include <unordered_map>

namespace nb = nanobind;

namespace artery::sionna::tests
{

TEST_F(MitsubaLLVMFixture, CasterSceneObjectRoundTrip)
{
    py::SceneObject object = getProvider<py::SceneObject>().next();

    nb::object pyObject = nb::cast(object);
    ASSERT_TRUE(pyObject.is_valid());
    ASSERT_TRUE(nb::isinstance<py::SceneObject>(pyObject));

    py::SceneObject roundTrip = nb::cast<py::SceneObject>(pyObject);
    EXPECT_EQ(roundTrip.object().ptr(), pyObject.ptr());
}

TEST_F(MitsubaLLVMFixture, CasterSceneObjectUnorderedMapRoundTrip)
{
    py::SceneObject object = getProvider<py::SceneObject>().next();
    std::unordered_map<std::string, py::SceneObject> objects{
        {"vehicle-1", object},
    };

    nb::object pyMap = nb::cast(objects);
    ASSERT_TRUE(pyMap.is_valid());

    auto roundTrip = nb::cast<std::unordered_map<std::string, py::SceneObject>>(pyMap);
    ASSERT_EQ(roundTrip.size(), 1u);
    ASSERT_TRUE(roundTrip.contains("vehicle-1"));
}

}  // namespace artery::sionna::tests
