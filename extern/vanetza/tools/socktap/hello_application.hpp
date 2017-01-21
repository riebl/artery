#ifndef HELLO_APPLICATION_HPP_EUIC2VFR
#define HELLO_APPLICATION_HPP_EUIC2VFR

#include "application.hpp"
#include <boost/asio/steady_timer.hpp>

class HelloApplication : public Application
{
public:
    HelloApplication(boost::asio::steady_timer&);
    PortType port() override;
    void indicate(const DataIndication&, UpPacketPtr) override;

private:
    void schedule_timer();
    void on_timer(const boost::system::error_code& ec);

    boost::asio::steady_timer& timer_;
};

#endif /* HELLO_APPLICATION_HPP_EUIC2VFR */

