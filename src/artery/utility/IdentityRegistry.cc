/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/utility/IdentityRegistry.h"
#include <omnetpp/checkandcast.h>

namespace artery
{

Define_Module(IdentityRegistry)

using namespace omnetpp;

const simsignal_t IdentityRegistry::updateSignal = cComponent::registerSignal("IdentityUpdated");
const simsignal_t IdentityRegistry::removeSignal = cComponent::registerSignal("IdentityRemoved");

void IdentityRegistry::initialize()
{
    getSystemModule()->subscribe(updateSignal, this);
    getSystemModule()->subscribe(removeSignal, this);
}

void IdentityRegistry::finish()
{
    getSystemModule()->unsubscribe(updateSignal, this);
    getSystemModule()->unsubscribe(removeSignal, this);
}

void IdentityRegistry::receiveSignal(cComponent*, simsignal_t signal, cObject* obj, cObject*)
{
    if (signal == updateSignal) {
        auto identity = check_and_cast<Identity*>(obj);
        auto& traci_index = mIdentities.get<traci>();
        auto found = traci_index.find(identity->traci);
        if (found != traci_index.end()) {
            mIdentities.replace(found, *identity);
        } else {
            mIdentities.insert(*identity);
        }
    } else if (signal == removeSignal) {
        auto identity = dynamic_cast<Identity*>(obj);
        if (identity) {
            auto& traci_index = mIdentities.get<traci>();
            traci_index.erase(identity->traci);
        }
    }
}

} // namespace artery
