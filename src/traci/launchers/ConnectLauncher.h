#pragma once

#include <traci/launchers/Launcher.h>
#include <omnetpp/csimplemodule.h>

namespace artery {

    namespace traci {

        /**
         * @brief This launcher just assumes connection to SUMO.
         */
        class ConnectLauncher 
            : public ILauncher
            , public omnetpp::cSimpleModule {
        public:
            /* omnetpp::cSimpleModule implementation */
            void initialize() override;

            /* ILauncher implementation */
            ServerEndpoint launch() override;

        private:
            ServerEndpoint endpoint_;
        };

    }

}
