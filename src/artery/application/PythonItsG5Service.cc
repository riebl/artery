#include "artery/application/PythonItsG5Service.h"

#include "omnetpp/cexception.h"
#include "pybind11/detail/common.h"
#include "pybind11/embed.h"
#include "pybind11/pybind11.h"
#include "pybind11/pytypes.h"

#include <omnetpp/cconfiguration.h>

#include <cstddef>
#include <filesystem>
#include <memory>

using namespace omnetpp;
namespace py = pybind11;

namespace artery
{

Define_Module(PythonItsG5Service)

class SharedPythonContext
{
public:
    static py::scoped_interpreter& interpreter()
    {
        if (!mInterpreter) {
            mInterpreter = std::make_shared<py::scoped_interpreter>();
        }
        return *mInterpreter;
    }

private:
    static std::shared_ptr<py::scoped_interpreter> mInterpreter;
};

std::shared_ptr<py::scoped_interpreter> SharedPythonContext::mInterpreter = nullptr;

struct PythonItsG5Service::PythonContext {
    py::module module;
    py::object service;
};

PythonItsG5Service::PythonItsG5Service()
{
}

void PythonItsG5Service::initialize(int stage)
{
    switch (stage) {
        case 0:
            return initializeInterpreter();
        case 1:
            return initializeModules();
    }
}

int PythonItsG5Service::numInitStages() const
{
    return 2;
}

void PythonItsG5Service::initializeInterpreter()
{
    if (const cModule* storyboard = getModuleByPath("World.storyboard"); storyboard != nullptr) {
        throw cRuntimeError("Could not initialize Python interpreter: this module conflicts with storyboard");
    }

    SharedPythonContext::interpreter();
    mPythonContext = std::make_unique<PythonContext>();
}

void PythonItsG5Service::initializeModules()
{
    try {
        const auto& netConfigEntry = getEnvir()->getConfig()->getConfigEntry("network");
        if (const char* simBaseDir = netConfigEntry.getBaseDirectory(); simBaseDir != nullptr) {
            extendPythonPath(simBaseDir);
        } else {
            throw cRuntimeError("Could not get scenario base directory: getBaseDirectory() on network entry returned nullptr");
        }

#ifdef PREFER_LOCAL_PYTHON_STUBS
        using path = std::filesystem::path;
        auto localStubsBaseDir = path(__FILE__).parent_path() / "python";
        extendPythonPath(localStubsBaseDir.string());
#endif
        // Otherwise python stubs must be provided by system.

        const char* pythonModule = nullptr;
        try {
            pythonModule = par("pythonModule").stringValue();
            mPythonContext->module = py::module::import(pythonModule);
        } catch (const py::import_error&) {
            throw cRuntimeError("Failed to find python module: %s", pythonModule);
        }

        const char* pythonServiceCls = nullptr;
        try {
            pythonServiceCls = par("pythonServiceClass").stringValue();
            mPythonContext->service = mPythonContext->module.attr(pythonServiceCls);
        } catch (const py::attribute_error&) {
            throw cRuntimeError("Failed to get service class: %s from Python module: %s", pythonServiceCls, pythonModule);
        }
    } catch (const py::error_already_set& error) {
        throw cRuntimeError("Python exception raised: %s", error.what());
    }
}

void PythonItsG5Service::extendPythonPath(const std::string& filepath)
{
    py::module::import("sys").attr("path").attr("append")(filepath);
    EV_INFO << "Appended " << filepath << " to Python system path" << std::endl;
}

bool PythonItsG5Service::requiresListener() const
{
    return true;
}

void PythonItsG5Service::trigger()
{
}

void PythonItsG5Service::addTransportDescriptor(const TransportDescriptor& td)
{
}


}  // namespace artery