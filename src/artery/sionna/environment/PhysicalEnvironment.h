#pragma once

#include <artery/sionna/bridge/Helpers.h>
#include <artery/sionna/bridge/bindings/Scene.h>
#include <artery/sionna/environment/api/SionnaAPI.h>
#include <artery/sionna/environment/config/dynamic/TraciDynamicSceneConfigProvider.h>
#include <artery/sionna/environment/config/scenes/IStaticSceneProvider.h>
#include <inet/environment/contract/IPhysicalEnvironment.h>
#include <omnetpp/csimplemodule.h>
#include <traci/Listener.h>

#include <memory>
#include <optional>

namespace artery::sionna
{

class Ground;

class PhysicalEnvironment : public inet::physicalenvironment::IPhysicalEnvironment, public omnetpp::cSimpleModule, public traci::Listener, public ISionnaAPI
{
public:
    PhysicalEnvironment() = default;

    // inet::physicalenvironment::IPhysicalEnvironment implementation
    inet::physicalenvironment::IObjectCache* getObjectCache() const override;
    inet::physicalenvironment::IGround* getGround() const override;

    const inet::Coord& getSpaceMin() const override;
    const inet::Coord& getSpaceMax() const override;
    const inet::physicalenvironment::IMaterialRegistry* getMaterialRegistry() const override;

    int getNumObjects() const override;
    const inet::physicalenvironment::IPhysicalObject* getObject(int index) const override;
    const inet::physicalenvironment::IPhysicalObject* getObjectById(int id) const override;

    void visitObjects(const inet::IVisitor* visitor, const inet::LineSegment& lineSegment) const override;

    const py::SionnaScene& scene() const;
    py::SionnaScene& scene() override;

    // ISionnaAPI implementation.
    mitsuba::ref<mi::Scene> miScene() override;
    const mi::ScalarPoint3f& worldMin() const override;
    const mi::ScalarPoint3f& worldMax() const override;
    bool setTxArray(const py::AntennaArray& array) override;
    bool setRxArray(const py::AntennaArray& array) override;
    IDynamicSceneConfigProxy* dynamicConfiguration() override;
    ICoordinateTransformProxy* coordinateTransform() override;
    IIDConverterProxy* IDConversion() override;

protected:
    int numInitStages() const override;
    void initialize(int stage) override;
    void finish() override;

private:
    void initializePythonRuntime();
    void initializeScene();
    void initializeSionnaAPI();
    void initializeCoordinateTransform();
    void traciInit() override;

    std::unique_ptr<ScopedInterpreter> interpreter_;
    std::optional<py::SionnaScene> scene_;
    std::unique_ptr<Ground> ground_;
    mi::ScalarPoint3f worldMin_;
    mi::ScalarPoint3f worldMax_;
    inet::Coord spaceMin_;
    inet::Coord spaceMax_;
    std::shared_ptr<IDynamicSceneConfigProxy> dynamicConfiguration_;
    std::shared_ptr<ICoordinateTransformProxy> coordinateTransform_;
    std::shared_ptr<IIDConverterProxy> IDConversion_;
};

}  // namespace artery::sionna
