#include "SionnaSceneVisualizer.h"

#include <artery/sionna/environment/PathLoss.h>
#include <artery/sionna/environment/config/dynamic/TraciDynamicSceneConfigProvider.h>
#include <inet/common/ModuleAccess.h>
#include <omnetpp/cexception.h>
#include <omnetpp/cxmlelement.h>

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <optional>
#include <sstream>

using namespace artery::sionna;

Define_Module(SionnaSceneVisualizer);

void SionnaSceneVisualizer::initialize()
{
    api_ = ISionnaAPI::get(this);
    renderParams_.outputDir = par("outputDir").stdstringValue();
    renderParams_.camera = par("camera").stdstringValue();
    renderParams_.spp = par("spp").intValue();
    renderParams_.width = par("width").intValue();
    renderParams_.height = par("height").intValue();

    pathLoss_ = inet::getModuleFromPar<PathLoss>(par("pathLoss"), this, true);
    pathLoss_->subscribe(PathLoss::pathsSolved, this);
}

void SionnaSceneVisualizer::finish()
{
    pathLoss_->unsubscribe(PathLoss::pathsSolved, this);
    api_ = nullptr;
}

void SionnaSceneVisualizer::renderFrame()
{
    std::optional<py::Paths> paths = pathLoss_->cachedPaths();

    const auto cameras = resolveCameras();
    for (const auto& [cameraId, camera] : cameras) {
        const auto filename = framePath(cameraId);
        std::filesystem::create_directories(filename.parent_path());
        api_->scene().renderToFile(camera, filename.string(), renderParams_.spp, renderParams_.width, renderParams_.height, std::nullopt, paths, false);
    }

    ++frameIndex_;
}

void SionnaSceneVisualizer::receiveSignal(
    omnetpp::cComponent* /* source */, omnetpp::simsignal_t signal, unsigned long /* value */, omnetpp::cObject* /* details */)
{
    if (signal != PathLoss::pathsSolved) {
        throw omnetpp::cRuntimeError("SionnaSceneVisualizer received unknown signal");
    }

    renderFrame();
}

std::vector<std::pair<std::string, py::Camera>> SionnaSceneVisualizer::resolveCameras() const
{
    omnetpp::cXMLElement* root = par("cameraConfig").xmlValue();
    if (root == nullptr) {
        throw omnetpp::cRuntimeError("SionnaSceneVisualizer cameraConfig does not resolve to XML; provide xmldoc(...) and select a camera id");
    }

    const char* selectedId = renderParams_.camera.empty() ? nullptr : renderParams_.camera.c_str();
    std::vector<std::pair<std::string, py::Camera>> result;

    for (omnetpp::cXMLElement* child = root->getFirstChild(); child != nullptr; child = child->getNextSibling()) {
        if (std::strcmp(child->getTagName(), "camera") != 0) {
            continue;
        }

        const char* id = child->getAttribute("id");
        if (selectedId != nullptr && id != nullptr && renderParams_.camera != id) {
            continue;
        } else if (selectedId != nullptr && id == nullptr) {
            continue;
        }

        const char* positionAttr = child->getAttribute("position");
        const char* orientationAttr = child->getAttribute("orientation");
        if (positionAttr == nullptr || orientationAttr == nullptr) {
            throw omnetpp::cRuntimeError("camera entry must define both position and orientation attributes");
        }

        double px, py, pz, ox, oy, oz;
        if (std::sscanf(positionAttr, "%lf %lf %lf", &px, &py, &pz) != 3) {
            throw omnetpp::cRuntimeError("invalid camera position '%s'", positionAttr);
        }

        if (std::sscanf(orientationAttr, "%lf %lf %lf", &ox, &oy, &oz) != 3) {
            throw omnetpp::cRuntimeError("invalid camera orientation '%s'", orientationAttr);
        }

        std::string effectiveId;
        if (id != nullptr && std::strlen(id) > 0) {
            effectiveId = id;
        } else if (selectedId != nullptr) {
            effectiveId = selectedId;
        } else {
            effectiveId = "camera";
        }

        result.emplace_back(effectiveId, py::Camera(mi::Point3f(px, py, pz), mi::Point3f(ox, oy, oz)));
    }

    if (result.empty()) {
        if (selectedId != nullptr) {
            throw omnetpp::cRuntimeError("could not resolve visualizer camera '%s' from cameraConfig", renderParams_.camera.c_str());
        }
        throw omnetpp::cRuntimeError("could not resolve any visualizer camera from cameraConfig");
    }

    return result;
}

std::filesystem::path SionnaSceneVisualizer::framePath(const std::string& cameraId) const
{
    std::ostringstream name;
    name << "frame-" << std::setw(6) << std::setfill('0') << frameIndex_ << ".png";
    return std::filesystem::path(renderParams_.outputDir) / cameraId / name.str();
}
