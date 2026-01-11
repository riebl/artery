#pragma once

#include <traci/Defs.h>

namespace artery {

    namespace traci {

        /**
         * @brief Interface for various SUMO Launchers.
         *
         * Launchers are expected to start SUMO in a particular way
         * and return endpoint for created instance.
         */
        class ILauncher {
        public:
            virtual ServerEndpoint launch() = 0;
        };

    }
}