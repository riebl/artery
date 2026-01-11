#pragma once

#include <string>

#include <traci/launchers/Launcher.h>
#include <omnetpp/csimplemodule.h>

namespace artery {

    namespace traci {

        class IArgumentFormatter {
        public:
            
        };

        class EnvirArgumentFormatter {
        public:

        };

        class PosixLauncher 
            : public ILauncher,
            public omnetpp::cSimpleModule {
        public:
            PosixLauncher();
            /* ILauncher implementation */
            ServerEndpoint launch() override;

        protected:
            struct BaseSumoArgs {
                std::string messageLog;
                std::string executable;
                std::string sumocfg;
                std::string extraOptions;
                int port;
                int seed;
            };

            std::vector<std::string> formatBaseSumoArgs(BaseSumoArgs sumoArgs);

            /* omnetpp::cSimpleModule implementation */
            void initialize() override;
            void finish() override;

        protected:
            BaseSumoArgs sumoArgs_;

            struct SumoTraceConfig {
                bool enabled;
                std::string traceFile;
            } traceConfig_;
        };

    }

}
