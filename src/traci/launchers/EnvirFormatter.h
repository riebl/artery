#pragma once

#include <omnetpp/csimplemodule.h>
#include <traci/launchers/Launcher.h>

#include <regex>
#include <string>

namespace artery
{

namespace traci
{

/**
 * @brief Formatter for SUMO command line values.
 *
 * Note, all children should be accessble for Omnet++ and therefore
 * be modules (this class itself is a module interface).
 */
class IArgumentFormatter
{
public:
    static constexpr int ERROR = -1;
    static constexpr int OK = 0;

    /**
     * @brief Format a single command line argument.
     *
     * This method should throw if it encounters internal error and return ERROR value
     * if user provided bad input. Return OK if formatting was performed.
     */
    virtual int format(std::string& argument) = 0;
};

class EnvirArgumentFormatter : public IArgumentFormatter, public omnetpp::cSimpleModule
{
public:
    EnvirArgumentFormatter();

    /* IArgumentFormatter implementation */
    virtual int format(std::string& argument) override;

protected:
    /* omnetpp::cSimpleModule implementation */
    void initialize() override;

private:
    std::regex pattern_;
    omnetpp::cConfigurationEx* cfg_;
};

}  // namespace traci

}  // namespace artery
