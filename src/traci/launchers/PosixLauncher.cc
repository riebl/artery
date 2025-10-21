#include <libsumo/Simulation.h>
#include <libsumo/TraCIConstants.h>

#include <omnetpp/checkandcast.h>
#include <omnetpp/cconfiguration.h>

#include "PosixLauncher.h"

using namespace artery::traci;

Define_Module(EnvirArgumentFormatter);

EnvirArgumentFormatter::EnvirArgumentFormatter()
    : cfg_(nullptr)
    , pattern_(R"(%(.*):envir%)")
{}

void EnvirArgumentFormatter::initialize() {
    if (const omnetpp::cSimulation* sim = omnetpp::getSimulation(); !sim) {
        throw omnetpp::cRuntimeError("failed to access simulation: no one currently runs");
    } else {
        cfg_ = sim->getEnvir()->getConfigEx();
    }
}

int EnvirArgumentFormatter::format(std::string& argument) {
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

Define_Module(PosixLauncher);

PosixLauncher::PosixLauncher()
    : startConfig_{
        .traceGetters = false
    }
{}

void PosixLauncher::initialize() {
#ifdef LIBTRACI
    EV_WARN 
        << "Artery was compiled with libtraci. If your primary goal is to start SUMO from Artery process, "
        << "it is recommended to compile with libsumo (in-process API) instead.";
#endif

    // Common argument group
    std::string messageLogFile = par("messageLogFile").stringValue();
    std::string executable = par("command").stringValue();
    std::string sumocfg = par("sumocfg").stringValue();
    int port = par("port");
    int seed = par("seed");
    
    // Startup options (API)
    startConfig_.label = par("label").stdstringValue();
    startConfig_.traceFile = par("traceFile").stringValue();
    startConfig_.traceGetters = par("traceGetters");

    // Extra options
    std::string extraOptions = par("extraOptions").stringValue();

    std::vector<std::string> arguments (
        {executable, "-c", sumocfg, "--seed", std::to_string(seed), "--remote-port", std::to_string(port), "--message-log", messageLogFile}
    );

    appendExtraOptions(arguments, extraOptions);
    formatArguments(arguments);
}

void PosixLauncher::appendExtraOptions(std::vector<std::string>& arguments, const std::string& extraOptions) {
    std::stringstream iss (extraOptions, std::ios::in);
    std::string current;

    while (std::getline(iss, current, ' ')) {
        arguments.push_back(current);
    }
}

void PosixLauncher::formatArguments(std::vector<std::string>& arguments) {
    int numFormatters = par("numFormatters");
    for (int i = 0; i < numFormatters; ++i) {
        omnetpp::cModule* m = getSubmodule("formatter", i);
        auto formatter = omnetpp::check_and_cast<IArgumentFormatter*>(m);
        
        for (std::string& argument : arguments) {
            if (int retval = formatter->format(argument); retval == IArgumentFormatter::ERROR) {
                throw omnetpp::cRuntimeError("failed to format argument: \"%s\", formatter %s returned error", argument.c_str(), m->getFullName());
            }
        }
    }
}

ServerEndpoint PosixLauncher::launch() {
    std::size_t port;

    if (startConfig_.traceFile.empty()) {
        std::tie(port, std::ignore) = compat::Simulation::start(startConfig_.args, -1, libsumo::DEFAULT_NUM_RETRIES, startConfig_.label);
    } else {
        std::tie(port, std::ignore) = compat::Simulation::start(startConfig_.args, -1, libsumo::DEFAULT_NUM_RETRIES, startConfig_.label, true, startConfig_.traceFile, startConfig_.traceGetters);
    }

    auto [_, version] = compat::Simulation::getVersion();
    EV_INFO << "Initialized connection to SUMO with " << ((compat::Simulation::isLibsumo()) ? "libsumo" : "libtraci") << ", version: " << version;

    return ServerEndpoint{
        .hostname = "localhost",
        .port = port,
        .clientId = 1,
        .retry = true
    };
}

void PosixLauncher::cleanup(const std::string& cleanupReason) {
    std::call_once(cleanupFlag_, [this, &cleanupReason]() {
        compat::Simulation::close(cleanupReason);
        EV_INFO << "closed SUMO connection";
    });
}

void PosixLauncher::finish() {
    cleanup("Omnet++ simulation finished");
}

PosixLauncher::~PosixLauncher() {
    cleanup("cleaning up: PosixLauncher was destroyed");
}
