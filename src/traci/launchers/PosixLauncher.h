#pragma once

#include <omnetpp/csimplemodule.h>
#include <traci/launchers/Launcher.h>

#include <mutex>
#include <string>

namespace artery
{

namespace traci
{

class PosixLauncher : public ILauncher, public omnetpp::cSimpleModule
{
public:
    PosixLauncher();
    ~PosixLauncher();

    /* ILauncher implementation */
    ServerEndpoint launch() override;

protected:
    /* omnetpp::cSimpleModule implementation */
    void initialize() override;
    void finish() override;

    void cleanup(const std::string& cleanupReason);
    void appendExtraOptions(std::vector<std::string>& arguments, const std::string& extraOptions);
    void formatArguments(std::vector<std::string>& arguments);

private:
    std::once_flag cleanupFlag_;

    struct SumoStartConfig {
        std::vector<std::string> args;
        std::string traceFile;
        std::string label;
        bool traceGetters;
    } startConfig_;
};

}  // namespace traci

}  // namespace artery
