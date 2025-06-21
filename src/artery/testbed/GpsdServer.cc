#include "artery/testbed/GpsdServer.h"
#include "artery/testbed/OtaInterfaceLayer.h"
#include "artery/traci/Cast.h"
#include <vanetza/gnss/nmea.hpp>
#include <vanetza/gnss/wgs84point.hpp>

namespace artery
{

GpsdServer::GpsdServer(const std::string& timebase, unsigned short port) :
    mSocket(mIoContext)
{
    mTimer.setTimebase(timebase);
    waitForListener(port);
}

GpsdServer::~GpsdServer()
{
    mSocket.close();
}

void GpsdServer::sendPositionFix(OtaInterfaceLayer& ota)
{
    auto pos = ota.getCurrentPosition();
    vanetza::Wgs84Point point(pos.latitude, pos.longitude);
    auto velocity = ota.getCurrentSpeed();
    auto angle = traci::angle_cast(ota.getCurrentHeading());
    auto time = vanetza::Clock::at(mTimer.getCurrentTime());

    std::string gprmcSentence = vanetza::nmea::gprmc(time, point, vanetza::units::NauticalVelocity(velocity),
            vanetza::units::TrueNorth(angle.degree * vanetza::units::true_north_degrees));
    gprmcSentence.append("\r\n");
    write(gprmcSentence);

    std::string gpgaaSentence = gpgga(time, point, vanetza::nmea::Quality::Simulation,
            vanetza::units::Length(1.0 * boost::units::si::meters));
    gpgaaSentence.append("\r\n");
    write(gpgaaSentence);
}

void GpsdServer::write(const std::string& sentence)
{
    boost::system::error_code ec;
    boost::asio::write(mSocket, boost::asio::buffer(sentence), boost::asio::transfer_all(), ec);
    if (ec) {
        throw omnetpp::cRuntimeError("unable to write on gpsd socket");
    }
}

void GpsdServer::waitForListener(unsigned short port)
{
    using boost::asio::ip::tcp;
    tcp::acceptor acceptor(mIoContext, tcp::endpoint(tcp::v4(), port));
    std::cout << "wait for gpsd" << std::endl;
    acceptor.accept(mSocket);
    std::cout << "gpsd connected" << std::endl;
}

} // namespace artery
