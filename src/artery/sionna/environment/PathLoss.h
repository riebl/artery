#pragma once

#include <artery/sionna/bridge/SionnaBridge.h>
#include <artery/sionna/environment/api/SionnaAPI.h>
#include <artery/sionna/environment/config/dynamic/DynamicSceneConfigListener.h>
#include <artery/sionna/environment/radio/SionnaRadioDeviceBase.h>
#include <inet/common/INETDefs.h>
#include <inet/physicallayer/pathloss/FreeSpacePathLoss.h>
#include <omnetpp/clistener.h>

#include <optional>
#include <unordered_map>

namespace artery::sionna
{

class PathLoss : public inet::physicallayer::FreeSpacePathLoss, public DynamicSceneConfigListener
{
public:
    static omnetpp::simsignal_t pathsSolved;

    // omnetpp::cSimpleModule implementation.
    int numInitStages() const override;
    void initialize(int stage) override;
    void finish() override;

    // DynamicSceneConfigListener implementation.
    void onDynamicSceneEdited() override;

    // inet::physicallayer::IPrintableObject implementation.
    std::ostream& printToStream(std::ostream& stream, int level) const override;

    // inet::physicallayer::IPathLoss implementation.
    double computePathLoss(const inet::physicallayer::ITransmission* transmission, const inet::physicallayer::IArrival* arrival) const override;
    double computePathLoss(
        const inet::physicallayer::IRadio* receiverRadio, const inet::physicallayer::ITransmission* transmission,
        const inet::physicallayer::IArrival* arrival) const;

    // Return calculated Paths object. It may not be computed yet or invalidated,
    // (normally when it becomes irrelevant to current state of simulation)
    // so user code should use pathsSolved signal to handle loss updates.
    const std::optional<py::Paths>& cachedPaths() const;

protected:
    void invalidateCachedPaths() const;
    void solveCachedPaths() const;

protected:
    mutable ISionnaAPI* api_ = nullptr;
    std::optional<py::PathSolver> solver_;

    mutable struct {
        std::optional<py::Paths> object;
        std::unordered_map<std::string, std::size_t> txIndices;
        std::unordered_map<std::string, std::size_t> rxIndices;
    } paths_;

    struct {
        double maxRange;
        bool includeLineOfSight;
        bool includeReflections;
        bool includeDiffractions;
        int maxReflectionDepth;
        int maxDiffractionDepth;
        int maxNumPathsPerSrc;
        int samplesPerSrc;
    } solverParams_;
};

}  // namespace artery::sionna
