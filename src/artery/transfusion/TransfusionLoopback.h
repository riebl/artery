#ifndef TRANSFUSIONLOOPBACK_H_I9ODUY1B
#define TRANSFUSIONLOOPBACK_H_I9ODUY1B

#include <omnetpp/csimplemodule.h>
#include <memory>

namespace artery
{

class TransfusionLoopback : public omnetpp::cSimpleModule
{
public:
    TransfusionLoopback();
    ~TransfusionLoopback();

    void initialize() override;
    void finish() override;
    void handleMessage(omnetpp::cMessage*) override;

private:
    class Context;
    std::shared_ptr<Context> mContext;
    omnetpp::cMessage* mTimer;
    unsigned mSumRxBytes;
};

} // namespace artery

#endif /* TRANSFUSIONLOOPBACK_H_I9ODUY1B */

