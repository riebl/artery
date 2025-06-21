#ifndef ARTERY_GPSDSERVER_H_1ONTYCQA
#define ARTERY_GPSDSERVER_H_1ONTYCQA

#include "artery/application/Timer.h"
#include <boost/asio.hpp>
#include <string>

namespace artery
{

class OtaInterfaceLayer;

class GpsdServer
{
public:
    /**
     * Construct gpsd server providing NMEA0183 sentences
     * \param timebase for simulation time (YYYY-MM-DD HH:mm:ss)
     * \param port TCP port where server accepts connection
     */
    GpsdServer(const std::string& timebase, unsigned short port);
    ~GpsdServer();
    void sendPositionFix(OtaInterfaceLayer&);

private:
    void write(const std::string& sentence);
    void waitForListener(unsigned short port);

    boost::asio::io_context mIoContext;
    boost::asio::ip::tcp::socket mSocket;
    Timer mTimer;
};

} // namespace artery

#endif /* ARTERY_GPSDSERVER_H_1ONTYCQA */

