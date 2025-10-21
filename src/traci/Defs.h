#pragma once

#include <stdint.h>

#include <libsumo/Simulation.h>
#include <libsumo/TraCIDefs.h>

namespace artery {

    namespace traci {

        /**
         * @brief Namespace alias for currently used libsumo/libtraci. Use this to access common API.
         * 
         */
        namespace compat = LIBSUMO_NAMESPACE;
        
        /**
         * @brief Geographical position.
         * 
         * Geographical position that is defined by pair of values (longitude, latitude),
         * both measured in degrees. Note, 3D Traci positions cannot be converted to this class.
         */
        class TraCIGeoPosition {
        public:
            TraCIGeoPosition() = default;
            TraCIGeoPosition(double longitude, double latitude);
            TraCIGeoPosition(const compat::TraCIPosition& position);

            compat::TraCIPosition convert2D();

            double longitude = compat::INVALID_DOUBLE_VALUE;
            double latitude = compat::INVALID_DOUBLE_VALUE;
        };

        /**
         * @brief Small helper metadata class for initializing connection to SUMO via libtraci.
         */
        struct ServerEndpoint {
            std::string hostname;
            std::size_t port;
            std::size_t clientId = 1;
            bool retry = false;
        };

        /**
        * @brief TraCI angle
        * - measured in degree
        * - 0 is headed north
        * - clockwise orientation (i.e. 90 heads east)
        * - range from 0 to 360
        */
        struct TraCIAngle
        {
            explicit TraCIAngle(double d) : degree(d) {}
            double degree;
        };

    }
}
