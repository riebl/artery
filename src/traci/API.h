#pragma once

#include <libsumo/libsumo.h>

#include "Defs.h"

namespace artery {

    namespace traci {

        class API : public libsumo::Simulation {
        public:
            using Version = std::pair<int, std::string>;

            /**
             * @brief Connect to SUMO instance with \Endpoint metadata.
             * 
             * @param endpoint Connection details.
             */
            void connectTraCI(const ServerEndpoint& endpoint);
        };

    }

} // namespace traci

