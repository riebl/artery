#ifndef ARTERY_PASSIVELOGGER_H_PMVCFRNA
#define ARTERY_PASSIVELOGGER_H_PMVCFRNA

#include <omnetpp/csimplemodule.h>

namespace artery
{

class PassiveLogger : public omnetpp::cSimpleModule
{
public:
    void handleMessage(omnetpp::cMessage*) override;
};

} // namespace artery

#endif /* ARTERY_PASSIVELOGGER_H_PMVCFRNA */

