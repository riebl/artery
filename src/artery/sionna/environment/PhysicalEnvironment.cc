// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include "PhysicalEnvironment.h"

#include "Ground.h"

#include <artery/sionna/environment/Compat.h>
#include <artery/sionna/environment/api/CoordinateTransform.h>
#include <artery/sionna/environment/api/DynamicSceneConfig.h>
#include <artery/sionna/environment/api/IDConverter.h>
#include <drjit-core/jit.h>
#include <inet/common/ModuleAccess.h>
#include <inet/environment/common/MaterialRegistry.h>
#include <mitsuba/render/scene.h>
#include <omnetpp/cexception.h>
#include <omnetpp/cmodule.h>
#include <omnetpp/cxmlelement.h>
#include <traci/API.h>
#include <traci/BasicNodeManager.h>

using namespace artery::sionna;

Define_Module(PhysicalEnvironment);

namespace
{

template <typename Matrix>
Matrix parseMatrix(omnetpp::cXMLElement* data)
{
    Matrix matrix;
    for (const auto& item : data->getElementsByTagName("element")) {
        std::size_t row = std::stoull(item->getAttribute("row"));
        std::size_t column = std::stoull(item->getAttribute("column"));
        matrix[row][column] = std::stod(item->getNodeValue());
    }

    return matrix;
}

mi::Vector3f parseTranslationVector(omnetpp::cXMLElement* data)
{
    return {
        std::stod(data->getFirstChildWithTag("x")->getNodeValue()), std::stod(data->getFirstChildWithTag("y")->getNodeValue()),
        std::stod(data->getFirstChildWithTag("z")->getNodeValue())};
}

}  // namespace

int PhysicalEnvironment::numInitStages() const
{
    return inet::NUM_INIT_STAGES;
}

void PhysicalEnvironment::initialize(int stage)
{
    switch (stage) {
        case inet::InitStages::INITSTAGE_LOCAL:
            subscribeTraCI(getSystemModule());
            initializePythonRuntime();
            break;
        case inet::InitStages::INITSTAGE_PHYSICAL_ENVIRONMENT:
            initializeScene();
            initializeSionnaAPI();
            break;
        default:
            break;
    }
}

void PhysicalEnvironment::finish()
{
    unsubscribeTraCI();
    ground_.reset();
    scene_.reset();
    interpreter_.reset();
    omnetpp::cSimpleModule::finish();
}

void PhysicalEnvironment::initializePythonRuntime()
{
    if (interpreter_) {
        return;
    }

    if (hasPar("virtualEnvironmentPath")) {
        interpreter_ = std::make_unique<ScopedInterpreter>(par("virtualEnvironmentPath").stdstringValue());
    } else {
        // clang-format off
        #if defined(SIONNA_VENV_HINT)
            interpreter_ = std::make_unique<ScopedInterpreter>(SIONNA_VENV_HINT);
        #else
            throw omnetpp::cRuntimeError("failed to initialize interpreter, python virtual environment is not set");
        #endif
        // clang-format on
    }

    nanobind::gil_scoped_acquire gil;
    auto mi = nanobind::module_::import_("mitsuba");

    // Variant is defined during compilation.
    using VariantName = mitsuba::detail::variant<mi::Float, mi::Spectrum>;
    mi.attr("set_variant")(VariantName::name);
}

void PhysicalEnvironment::initializeScene()
{
    auto* provider = omnetpp::check_and_cast<IStaticSceneProvider*>(getSubmodule("sceneConfigProvider"));
    auto cppScene = provider->getSceneConfig();
    const auto& bbox = cppScene->bbox();
    worldMin_ = bbox.min;
    worldMax_ = bbox.max;
    spaceMin_ = convert<inet::Coord>(worldMin_);
    spaceMax_ = convert<inet::Coord>(worldMax_);
    scene_.emplace(std::move(cppScene));
}

void PhysicalEnvironment::initializeSionnaAPI()
{
    dynamicConfiguration_ = std::make_shared<DynamicSceneConfigProxy>(this);
    IDConversion_ = std::make_shared<TraciIDConverterProxy>();
    ground_ = std::make_unique<Ground>(this);
}

void PhysicalEnvironment::initializeCoordinateTransform()
{
    auto traciNodeManagerPath = par("traciNodeManagerModule").stdstringValue();
    auto* traciNodeManagerModule = getModuleByPath(traciNodeManagerPath.c_str());
    auto* traciNodeManager = dynamic_cast<traci::BasicNodeManager*>(traciNodeManagerModule);

    if (traciNodeManager == nullptr) {
        throw omnetpp::cRuntimeError("No TraCI node manager found at path %s", traciNodeManagerPath.c_str());
    }

    auto transform = std::make_shared<AffineCoordinateTransform>(
        parseMatrix<mi::Matrix3f>(par("remapMatrix").xmlValue()), parseMatrix<mi::Matrix2f>(par("rotationMatrix").xmlValue()),
        parseTranslationVector(par("translationVector").xmlValue()), traci::Boundary(traciNodeManager->getAPI()->simulation.getNetBoundary()), this);

    coordinateTransform_ = std::move(transform);
}

void PhysicalEnvironment::traciInit()
{
    initializeCoordinateTransform();
}

inet::physicalenvironment::IObjectCache* PhysicalEnvironment::getObjectCache() const
{
    // NOTE: Probably useless for sionna.
    return nullptr;
}

inet::physicalenvironment::IGround* PhysicalEnvironment::getGround() const
{
    return ground_.get();
}

const inet::Coord& PhysicalEnvironment::getSpaceMin() const
{
    return spaceMin_;
}

const inet::Coord& PhysicalEnvironment::getSpaceMax() const
{
    return spaceMax_;
}

const inet::physicalenvironment::IMaterialRegistry* PhysicalEnvironment::getMaterialRegistry() const
{
    return &inet::physicalenvironment::MaterialRegistry::singleton;
}

int PhysicalEnvironment::getNumObjects() const
{
    return 0;
}

const inet::physicalenvironment::IPhysicalObject* PhysicalEnvironment::getObject(int /* index */) const
{
    return nullptr;
}

const inet::physicalenvironment::IPhysicalObject* PhysicalEnvironment::getObjectById(int /* id */) const
{
    return nullptr;
}

void PhysicalEnvironment::visitObjects(const inet::IVisitor* visitor, const inet::LineSegment& lineSegment) const
{
    // NOTE: do nothing.
}

const artery::sionna::py::SionnaScene& PhysicalEnvironment::scene() const
{
    return scene_.value();
}

artery::sionna::py::SionnaScene& PhysicalEnvironment::scene()
{
    return scene_.value();
}

mitsuba::ref<mi::Scene> PhysicalEnvironment::miScene()
{
    return scene().miScene();
}

const mi::ScalarPoint3f& PhysicalEnvironment::worldMin() const
{
    return worldMin_;
}

const mi::ScalarPoint3f& PhysicalEnvironment::worldMax() const
{
    return worldMax_;
}

bool PhysicalEnvironment::setTxArray(const py::AntennaArray& array)
{
    if (scene().txArray().has_value()) {
        return false;
    }

    scene().setTxArray(array);
    return true;
}

bool PhysicalEnvironment::setRxArray(const py::AntennaArray& array)
{
    if (scene().rxArray().has_value()) {
        return false;
    }

    scene().setRxArray(array);
    return true;
}

IDynamicSceneConfigProxy* PhysicalEnvironment::dynamicConfiguration()
{
    return dynamicConfiguration_.get();
}

ICoordinateTransformProxy* PhysicalEnvironment::coordinateTransform()
{
    return coordinateTransform_.get();
}

IIDConverterProxy* PhysicalEnvironment::IDConversion()
{
    return IDConversion_.get();
}
