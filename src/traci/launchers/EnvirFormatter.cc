#include "EnvirFormatter.h"

#include <omnetpp/cconfiguration.h>

using namespace artery::traci;

Define_Module(EnvirArgumentFormatter);

EnvirArgumentFormatter::EnvirArgumentFormatter() : cfg_(nullptr), pattern_(R"(%(.*):envir%)")
{
}

void EnvirArgumentFormatter::initialize()
{
    if (const omnetpp::cSimulation* sim = omnetpp::getSimulation(); !sim) {
        throw omnetpp::cRuntimeError("failed to access simulation: no one currently runs");
    } else {
        cfg_ = sim->getEnvir()->getConfigEx();
    }
}

int EnvirArgumentFormatter::format(std::string& argument)
{
    constexpr std::size_t matchSize = 2;

    std::sregex_iterator it, end;
    std::vector<std::string> substitutions;

    for (it = std::sregex_iterator(argument.begin(), argument.end(), pattern_); it != end; ++it) {
        std::smatch match = *it;

        if (match.size() != matchSize) {
            throw omnetpp::cRuntimeError("failed to match: expected %d groups, but got: %d", matchSize, match.size());
        } else {
            substitutions.push_back(match[0].str());
        }
    }

    for (const std::string& envirVariable : substitutions) {
        if (const char* value = cfg_->getVariable(envirVariable.c_str()); !value) {
            EV_WARN << "could not find a variable with name = \"" << envirVariable << "\" in config";
            return ERROR;
        } else if (char pattern[256]; std::snprintf(pattern, sizeof(pattern), "%%%s%%", envirVariable.c_str()) < 0) {
            throw omnetpp::cRuntimeError("failed to format new pattern for replacement (probably buffer is too small?)");
        } else {
            std::regex_replace(argument, std::regex(pattern), value);
        }
    }

    return OK;
}