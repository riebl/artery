// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include "Scene.h"

#include <artery/sionna/bridge/Casters.h>
#include <artery/sionna/bridge/Helpers.h>
#include <mitsuba/render/scene.h>
#include <nanobind/stl/string.h>

using namespace artery::sionna;
using namespace artery::sionna::literals;

namespace
{

template <typename T>
std::vector<std::pair<std::string, T>> orderedItems(const nanobind::object& object, const char* attribute)
{
    std::vector<std::pair<std::string, T>> result;
    nanobind::dict items(object.attr(attribute));
    result.reserve(items.size());

    for (const auto& [key, value] : items) {
        result.emplace_back(nanobind::cast<std::string>(key), nanobind::cast<T>(value));
    }

    return result;
}

}  // namespace

py::SionnaScene::SionnaScene()
{
    InitPythonClassCapability::init();
}

py::SionnaScene::SionnaScene(nanobind::object obj)
{
    WrapPythonClassCapability::init(std::move(obj));
}

py::SionnaScene::SionnaScene(mitsuba::ref<mi::Scene> scene)
{
    InitPythonClassCapability::init("mi_scene"_a = std::move(scene));
}

const char* py::SionnaScene::className() const
{
    return "Scene";
}

void py::SionnaScene::edit(const std::vector<SceneObject>& add, const std::vector<std::string>& remove)
{
    sionna::call(bound_, "edit", "add"_a = add, "remove"_a = remove);
}

void py::SionnaScene::add(const Transmitter& transmitter)
{
    sionna::call(bound_, "add", "item"_a = transmitter);
}

void py::SionnaScene::add(const Receiver& receiver)
{
    sionna::call(bound_, "add", "item"_a = receiver);
}

void py::SionnaScene::remove(const std::string& name)
{
    sionna::call(bound_, "remove", "name"_a = name);
}

void py::SionnaScene::renderToFile(
    std::variant<std::string, Camera> camera, const std::string& filename, int numSamples, int width, int height, std::optional<float> fov,
    std::optional<Paths> paths, bool showDevices) const
{
    sionna::call(
        bound_, "render_to_file", "camera"_a = std::move(camera), "filename"_a = filename, "num_samples"_a = numSamples,
        "resolution"_a = std::make_tuple(width, height), "fov"_a = fov, "paths"_a = paths, "show_devices"_a = showDevices);
}

py::SionnaScene::TSceneElement py::SionnaScene::get(const std::string& name) const
{
    nanobind::object value = sionna::call<nanobind::object>(bound_, "get", "name"_a = name);

    if (value.is_none()) {
        return std::monostate();
    } else if (nanobind::isinstance<py::SceneObject>(value)) {
        return nanobind::cast<py::SceneObject>(value);
    } else if (nanobind::isinstance<py::RadioMaterial>(value)) {
        return nanobind::cast<py::RadioMaterial>(value);
    } else if (nanobind::isinstance<py::Transmitter>(value)) {
        return nanobind::cast<py::Transmitter>(value);
    } else if (nanobind::isinstance<py::Receiver>(value)) {
        return nanobind::cast<py::Receiver>(value);
    }

    throw sionna::wrapRuntimeError("could not get object from scene: unsupported item type");
}

std::unordered_map<std::string, py::RadioMaterial> py::SionnaScene::radioMaterials() const
{
    return sionna::access<std::unordered_map<std::string, py::RadioMaterial>>(bound_, "_radio_materials");
}

std::unordered_map<std::string, py::SceneObject> py::SionnaScene::sceneObjects() const
{
    return sionna::access<std::unordered_map<std::string, py::SceneObject>>(bound_, "_scene_objects");
}

std::unordered_map<std::string, py::Transmitter> py::SionnaScene::transmitters() const
{
    return sionna::access<std::unordered_map<std::string, py::Transmitter>>(bound_, "_transmitters");
}

py::SionnaScene::TTransmitters py::SionnaScene::orderedTransmitters() const
{
    return orderedItems<py::Transmitter>(bound_, "_transmitters");
}

std::unordered_map<std::string, py::Receiver> py::SionnaScene::receivers() const
{
    return sionna::access<std::unordered_map<std::string, py::Receiver>>(bound_, "_receivers");
}

py::SionnaScene::TReceivers py::SionnaScene::orderedReceivers() const
{
    return orderedItems<py::Receiver>(bound_, "_receivers");
}

std::optional<py::AntennaArray> py::SionnaScene::txArray() const
{
    return sionna::access<std::optional<py::AntennaArray>>(bound_, "tx_array");
}

void py::SionnaScene::setTxArray(const AntennaArray& array)
{
    sionna::set(bound_, "tx_array", array);
}

std::optional<py::AntennaArray> py::SionnaScene::rxArray() const
{
    return sionna::access<std::optional<py::AntennaArray>>(bound_, "rx_array");
}

void py::SionnaScene::setRxArray(const AntennaArray& array)
{
    sionna::set(bound_, "rx_array", array);
}

maybe_diff_t<mi::Float> py::SionnaScene::frequency() const
{
    return sionna::access<maybe_diff_t<mi::Float>>(bound_, "frequency");
}

void py::SionnaScene::setFrequency(maybe_diff_t<mi::Float> f)
{
    sionna::set(bound_, "frequency", f);
}

maybe_diff_t<mi::Float> py::SionnaScene::wavelength() const
{
    return sionna::access<maybe_diff_t<mi::Float>>(bound_, "wavelength");
}

maybe_diff_t<mi::Float> py::SionnaScene::wavenumber() const
{
    return sionna::access<maybe_diff_t<mi::Float>>(bound_, "wavenumber");
}

maybe_diff_t<mi::Float> py::SionnaScene::bandwidth() const
{
    return sionna::access<maybe_diff_t<mi::Float>>(bound_, "bandwidth");
}

void py::SionnaScene::setBandwidth(maybe_diff_t<mi::Float> bw)
{
    sionna::set(bound_, "bandwidth", bw);
}

maybe_diff_t<mi::Float> py::SionnaScene::temperature() const
{
    return sionna::access<maybe_diff_t<mi::Float>>(bound_, "temperature");
}

void py::SionnaScene::setTemperature(maybe_diff_t<mi::Float> t)
{
    sionna::set(bound_, "temperature", t);
}

mitsuba::ref<mi::Scene> py::SionnaScene::miScene() const
{
    return sionna::access<mitsuba::ref<mi::Scene>>(bound_, "_scene");
}
