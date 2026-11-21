#pragma once

#include <artery/sionna/bridge/Fwd.h>
#include <artery/sionna/bridge/bindings/AntennaArray.h>
#include <artery/sionna/bridge/bindings/RadioDevice.h>
#include <artery/sionna/bridge/bindings/Scene.h>
#include <artery/sionna/bridge/bindings/SceneObject.h>
#include <drjit/array.h>
#include <omnetpp/cexception.h>
#include <omnetpp/cmodule.h>
#include <traci/Angle.h>
#include <traci/Position.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace artery::sionna
{

using CoordinateArray3f = drjit::Array<mi::Float, 3>;

// Different places that define IDs. Note, these
// are created mainly to sync objects in various simulators.
enum class IDNamespace : std::uint8_t {
    SUMO,
    SIONNA,
};

// This proxy class exposes scene layout operations that are processed in batch.
class IDynamicSceneConfigProxy
{
public:
    // This class binds to objects which is queued to be added and allows to also
    // queue geometric transforms.
    class ITransformProxy
    {
    public:
        // Apply post-edit transform to object, mainly used for scene geometry.
        virtual void transform(std::function<void(py::SceneObject&)> function) = 0;

        virtual ~ITransformProxy() = default;
    };

    // Queues object to add.
    virtual std::unique_ptr<ITransformProxy> addObject(py::SceneObject object) = 0;

    // Queues transmitter to add if no scene item with the same ID exists.
    virtual bool addTransmitter(py::Transmitter transmitter) = 0;

    // Queues receiver to add if no scene item with the same ID exists.
    virtual bool addReceiver(py::Receiver receiver) = 0;

    // Queues existing object update.
    virtual std::unique_ptr<ITransformProxy> updateObject(const std::string& id) = 0;

    // Removes scene item by scene ID. Physical objects are queued, radio devices are removed immediately.
    virtual void removeSceneItem(const std::string& id) = 0;

    // Flush pending changes. All queued objects to add are added, all queued
    // objects to remove are removed. Then all transforms are called. Invalidates
    // mi.Scene, since mitsuba scene is rebuilt on this action.
    virtual void edit() = 0;

    virtual ~IDynamicSceneConfigProxy() = default;
};

// This proxy handles coordinate transforms. Coordinates consist of 3 components (x, y, z).
class ICoordinateTransformProxy
{
public:
    // Converts SUMO position coordinates into remapped Sionna scene coordinates used by scene objects.
    virtual mi::Point3f fromSumo(const libsumo::TraCIPosition& position) = 0;

    // Converts SUMO heading into remapped Sionna scene orientation coordinates.
    virtual mi::Point3f fromSumo(traci::TraCIAngle heading) = 0;

    // Applies the Sionna scene remap to canonical scene coordinates.
    virtual CoordinateArray3f toSionnaScene(const CoordinateArray3f& v) = 0;

    // Reverses the Sionna scene remap, returning canonical scene coordinates.
    virtual CoordinateArray3f fromSionnaScene(const CoordinateArray3f& v) = 0;

    // When transferring object from SUMO or other 2D simulators, use this function
    // to place object on terrain or other mesh, like road for example.
    virtual void adjustVerticalComponent(py::SceneObject& object) = 0;

    virtual ~ICoordinateTransformProxy() = default;
};

// This proxy handles ID conversions.
class IIDConverterProxy
{
public:
    // Currently, we need to be able to convert IDs between SUMO and Sionna, since
    // requirements for those IDs differ a bit. Ideally, use the same ID everywhere for single entity
    // if possible. Commonly ID transforms are implemented as bidirectional maps.
    virtual std::string convertID(IDNamespace from, IDNamespace to, const std::string& id) = 0;

    // Remove cached ID mapping by ID namespace.
    virtual void removeID(IDNamespace ns, const std::string& id) = 0;

    virtual ~IIDConverterProxy() = default;
};

// This is an API class for common utility methods, used while
// interacting with Sionna. This class may be configured via proxy
// classes to alter common behavior. The reason why this exists is because
// replacing NED modules via config is not very convenient, so instead
// this can be done by overloading a single initialize() here.
class ISionnaAPI
{
public:
    // Gets API owner module, errors in case it was not found.
    static ISionnaAPI* get(const omnetpp::cModule* module);

    /****************
     * Scene basics *
     ****************
     */

    // Access sionna scene object.
    virtual py::SionnaScene& scene() = 0;

    // Faster path to access mitsuba scene, currently used by Sionna. Note,
    // on scene edits it changes.
    virtual mitsuba::ref<mi::Scene> miScene() = 0;

    // Axis-aligned bounds of the current Mitsuba scene.
    virtual const mi::ScalarPoint3f& worldMin() const = 0;
    virtual const mi::ScalarPoint3f& worldMax() const = 0;

    // Configure scene-wide transmitter array if not configured yet.
    virtual bool setTxArray(const py::AntennaArray& array) = 0;

    // Configure scene-wide receiver array if not configured yet.
    virtual bool setRxArray(const py::AntennaArray& array) = 0;

    /**************************************
     * Scene configuration and operations *
     **************************************
     */

    // Access proxy class to make dynamic changes to the scene.
    virtual IDynamicSceneConfigProxy* dynamicConfiguration() = 0;

    // Access proxy class to complete various coordinate operations.
    virtual ICoordinateTransformProxy* coordinateTransform() = 0;

    // Access proxy class for converting object IDs.
    virtual IIDConverterProxy* IDConversion() = 0;

    virtual ~ISionnaAPI() = default;
};

}  // namespace artery::sionna
