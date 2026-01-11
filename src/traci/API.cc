#include <libsumo/Simulation.h>
#include <libsumo/TraCIConstants.h>

#include <omnetpp/cexception.h>

#include "API.h"

using namespace artery;

void traci::API::connectTraCI(const ServerEndpoint& endpoint) {
#ifdef LIBTRACI
    std::size_t numRetries = (endpoint.retry) ? compat::DEFAULT_NUM_RETRIES : 0;
    compat::Simulation::init(endpoint.port, numRetries, endpoint.hostname);
    compat::Simulation::setOrder(endpoint.clientId);
#else
    throw omnetpp::cRuntimeError("libsumo does not allow connecting to existing endpoint. Please compile with LIBTRACI instead");
#endif
}
