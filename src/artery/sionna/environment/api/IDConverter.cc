#include "IDConverter.h"

#include <artery/sionna/bridge/Helpers.h>
#include <omnetpp/cexception.h>

#include <algorithm>

using namespace artery::sionna;

std::string TraciIDConverterProxy::convertID(IDNamespace from, IDNamespace to, const std::string& id)
{
    if (from == to) {
        return id;
    }

    if (from == IDNamespace::SUMO && to == IDNamespace::SIONNA) {
        return sionnaID(id);
    }

    if (from == IDNamespace::SIONNA && to == IDNamespace::SUMO) {
        return sumoID(id);
    }

    throw omnetpp::cRuntimeError("unsupported ID conversion from %d to %d", static_cast<int>(from), static_cast<int>(to));
}

void TraciIDConverterProxy::removeID(IDNamespace ns, const std::string& id)
{
    switch (ns) {
        case IDNamespace::SUMO:
            removeBySumoID(id);
            return;
        case IDNamespace::SIONNA:
            removeBySionnaID(id);
            return;
    }

    throw omnetpp::cRuntimeError("unsupported ID namespace for removal: %d", static_cast<int>(ns));
}

std::string TraciIDConverterProxy::sumoID(const std::string& sionnaID) const
{
    const auto& sionnaIDs = mapping_.right;
    if (auto iter = sionnaIDs.find(sionnaID); iter != sionnaIDs.end()) {
        return iter->get_left();
    }

    throw omnetpp::cRuntimeError("failed to get SUMO ID: no Sionna ID found: %s", sionnaID.c_str());
}

std::string TraciIDConverterProxy::sionnaID(const std::string& sumoID)
{
    auto& sumoIDs = mapping_.left;
    auto& sionnaIDs = mapping_.right;

    if (auto iter = sumoIDs.find(sumoID); iter != sumoIDs.end()) {
        return iter->get_right();
    }

    std::string converted = sumoID;
    if (converted.empty()) {
        throw omnetpp::cRuntimeError("could not convert SUMO ID: converted ID is empty");
    }

    // Dots are not allowed in Sionna scene names; underscores are fine.
    std::replace(converted.begin(), converted.end(), '.', '_');

    const std::string base = converted;
    for (std::size_t repeatedTimes = 0; sionnaIDs.find(converted) != sionnaIDs.end(); ++repeatedTimes) {
        converted = format("%s_%zu", base.c_str(), repeatedTimes);
    }

    mapping_.insert({sumoID, converted});
    return converted;
}

void TraciIDConverterProxy::removeBySumoID(const std::string& id)
{
    auto& sumoIDs = mapping_.left;
    if (auto iter = sumoIDs.find(id); iter != sumoIDs.end()) {
        sumoIDs.erase(iter);
        return;
    }

    throw omnetpp::cRuntimeError("failed to remove ID: no SUMO ID found: %s", id.c_str());
}

void TraciIDConverterProxy::removeBySionnaID(const std::string& id)
{
    auto& sionnaIDs = mapping_.right;
    if (auto iter = sionnaIDs.find(id); iter != sionnaIDs.end()) {
        sionnaIDs.erase(iter);
        return;
    }

    throw omnetpp::cRuntimeError("failed to remove ID: no Sionna ID found: %s", id.c_str());
}
