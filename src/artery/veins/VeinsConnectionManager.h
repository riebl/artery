#ifndef VEINSCONNECTIONMANAGER_H_KARMFGK9
#define VEINSCONNECTIONMANAGER_H_KARMFGK9

#include <veins/base/connectionManager/ConnectionManager.h>
#include <omnetpp/clistener.h>

namespace artery
{

/**
 * Slightly extended variant of Veins' ConnectionManager.
 *
 * This variant takes care to unregister NICs from the connection manager
 * when the respective nodes (vehicles) are removed from OMNeT++.
 * Originally,this is done by Veins' TraCIScenarioManager.
 * Note: NICs register automatically at the networks' connection manager.
 */
class VeinsConnectionManager : public veins::ConnectionManager, public omnetpp::cListener
{
public:
    void initialize(int stage) override;
    void finish() override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, const char*, omnetpp::cObject*) override;
};

} // namespace artery

#endif /* VEINSCONNECTIONMANAGER_H_KARMFGK9 */

