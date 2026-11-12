#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/bindings/AntennaArray.h>
#include <artery/sionna/bridge/bindings/Camera.h>
#include <artery/sionna/bridge/bindings/Material.h>
#include <artery/sionna/bridge/bindings/Paths.h>
#include <artery/sionna/bridge/bindings/RadioDevice.h>
#include <artery/sionna/bridge/bindings/SceneObject.h>
#include <artery/sionna/bridge/capabilities/Calling.h>

#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace artery::sionna::py
{

// Wrapper for mitsuba scene that handles querying objects,
// rendering, setting properties for propagating signals, etc.
class SIONNA_BRIDGE_API SionnaScene : public SionnaRtModule, public InitPythonClassCapability
{
public:
    // Result of scene objects query.
    using TSceneElement = std::variant<std::monostate, SceneObject, RadioMaterial, Transmitter, Receiver>;
    using TTransmitters = std::vector<std::pair<std::string, Transmitter>>;
    using TReceivers = std::vector<std::pair<std::string, Receiver>>;

    // IPythonClassIdentityCapability implementation.
    const char* className() const override;

    // Default constructor, does not hold valid object yet.
    SionnaScene();
    // Assume passed object is of type SionnaScene.
    explicit SionnaScene(nanobind::object obj);
    // Wrap mitsuba scene.
    explicit SionnaScene(mitsuba::ref<mi::Scene> scene);

    // Reconstruct the scene, adding objects from add array and removing objects by id. This
    // invalidates mi.Scene ref you may have accessed earlier, be careful.
    void edit(const std::vector<SceneObject>& add, const std::vector<std::string>& remove);

    // Add transmitter to the scene. Tx devices are not present in scene physically, so adding them
    // is as simple as this can be.
    void add(const Transmitter& transmitter);

    // Add rx device, similar to adding tx.
    void add(const Receiver& receiver);

    // Remove rx, tx or material from the scene. For objects, use edit().
    void remove(const std::string& name);

    // Render scene to a file - most adopted way here to get images. For full doc, please
    // refer to original render() call, but basically:
    // 1) camera is either already present camera object or new one you just created from which scene
    // will be rendered
    // 2) numSamples, width, height - control resulting image quality
    // 3) paths are calculated signal paths to visualize
    // 4) showDevices adds big ball-like indications to tx and rx devices.
    void renderToFile(
        std::variant<std::string, Camera> camera, const std::string& filename, int numSamples = 512, int width = 655, int height = 500,
        std::optional<float> fov = std::nullopt, std::optional<Paths> paths = std::nullopt, bool showDevices = true) const;

    // Query object from the scene, either physical like objects or Sionna components like radio devices.
    TSceneElement get(const std::string& name) const;

    // The following methods all access protected fields. I'm not sure if it's a good
    // idea to do so, but there is no other way around, really.

    // Builds mapping from ID to radio materials. Don't call this often.
    std::unordered_map<std::string, RadioMaterial> radioMaterials() const;
    // Builds mapping from ID to scene objects. Don't call this often.
    std::unordered_map<std::string, SceneObject> sceneObjects() const;
    // Builds mapping from ID to tx devices. Don't call this often.
    std::unordered_map<std::string, Transmitter> transmitters() const;
    // Builds ordered tx entries matching Sionna path tensor indices.
    TTransmitters orderedTransmitters() const;
    // Builds mapping from ID to rx devices. Don't call this often.
    std::unordered_map<std::string, Receiver> receivers() const;
    // Builds ordered rx entries matching Sionna path tensor indices.
    TReceivers orderedReceivers() const;

    // Access tx array. If None, return std::nullopt.
    std::optional<AntennaArray> txArray() const;
    // Sets tx array globally. Note, all radio devices use this.
    void setTxArray(const AntennaArray& array);

    // Access rx array. If None, return std::nullopt.
    std::optional<AntennaArray> rxArray() const;
    // Sets rx array globally. Note, all radio devices use this.
    void setRxArray(const AntennaArray& array);

    // Access frequency setting for transmissions. All transmissions are done on the same frequency.
    maybe_diff_t<mi::Float> frequency() const;
    // Sets frequency for all radio devices.
    void setFrequency(maybe_diff_t<mi::Float> f);

    // Wavelength calculated from frequency.
    maybe_diff_t<mi::Float> wavelength() const;
    // Wavenumber, calculated from wavelength.
    maybe_diff_t<mi::Float> wavenumber() const;

    // Access transmissions bandwidth. Same for all transmissions.
    maybe_diff_t<mi::Float> bandwidth() const;
    // Sets bandwidth for all transmissions.
    void setBandwidth(maybe_diff_t<mi::Float> bw);

    // Access temperature.
    maybe_diff_t<mi::Float> temperature() const;
    // Sets temperature.
    void setTemperature(maybe_diff_t<mi::Float> t);

    // Access current mitsuba scene. Each scene editing invalidates this ref.
    mitsuba::ref<mi::Scene> miScene() const;
};

}  // namespace artery::sionna::py
