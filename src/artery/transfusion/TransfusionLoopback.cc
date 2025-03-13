#include "TransfusionLoopback.h"
#include <boost/asio.hpp>
#include <omnetpp/cmessage.h>
#include <array>

using namespace omnetpp;

namespace artery
{

Define_Module(TransfusionLoopback)

class TransfusionLoopback::Context
{
public:
    Context(int port) :
        mEndpoint(boost::asio::ip::tcp::v4(), port),
        mAcceptor(mIoContext, mEndpoint),
        mSocket(mIoContext)
    {
        mAcceptor.listen();
        mAcceptor.async_accept(mSocket,
                [this](const boost::system::error_code& ec) {
                    read();
                });
    }

    std::size_t receive()
    {
        mRxBytes = 0;
        mIoContext.poll();
        return mRxBytes;
    }

private:
    void read()
    {
        mSocket.async_read_some(boost::asio::buffer(mBuffer),
                [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                    mRxBytes += bytes_transferred;
                    read();
                });
    }

    boost::asio::io_context mIoContext;
    boost::asio::ip::tcp::endpoint mEndpoint;
    boost::asio::ip::tcp::acceptor mAcceptor;
    boost::asio::ip::tcp::socket mSocket;
    std::array<uint8_t, 1024> mBuffer;
    std::size_t mRxBytes = 0;
};

TransfusionLoopback::TransfusionLoopback() :
    mTimer(new omnetpp::cMessage("loopback timer")),
    mSumRxBytes(0)
{
}

TransfusionLoopback::~TransfusionLoopback()
{
    cancelAndDelete(mTimer);
}

void TransfusionLoopback::initialize()
{
    mContext = std::make_shared<Context>(par("port"));
    scheduleAt(simTime(), mTimer);
}

void TransfusionLoopback::finish()
{
    recordScalar("sumRxBytes", mSumRxBytes);
}

void TransfusionLoopback::handleMessage(omnetpp::cMessage* msg)
{
    if (msg->isSelfMessage()) {
        const std::size_t bytes = mContext->receive();
        mSumRxBytes += bytes;
        EV_INFO << "received " << bytes << " bytes\n";
        scheduleAt(simTime() + SimTime { 100, SIMTIME_MS }, mTimer);
    }
}

} // namespace artery
