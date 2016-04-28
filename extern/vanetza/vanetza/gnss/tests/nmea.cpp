#include <gtest/gtest.h>
#include <vanetza/gnss/nmea.hpp>
#include <vanetza/gnss/wgs84point.hpp>
#include <string>

using namespace vanetza;

TEST(NMEA, gprmc) {
    using namespace boost::posix_time;
    using namespace boost::gregorian;
    nmea::time time(date(2013, 12, 5), time_duration(10, 32, 15));
    Wgs84Point position(49.8234932 * units::degree, -12.5343 * units::degree);
    units::NauticalVelocity speed(9.358 * units::metric::knots);
    units::TrueNorth heading(273.4 * units::true_north_degrees);

    std::string rmc = nmea::gprmc(time, position, speed, heading);
    EXPECT_EQ("$GPRMC,103215,A,4949.4096,N,1232.0580,W,9.4,273.4,051213,1.0,E,A*33", rmc);
}

TEST(NMEA, gpgga) {
    using namespace boost::posix_time;
    using namespace boost::gregorian;
    nmea::time time(date(2013, 11, 30), time_duration(19, 55, 59));
    Wgs84Point position(-0.34 * units::degree, 165.39999 * units::degree);
    nmea::Quality quality = nmea::Quality::DGPS;
    units::Length hdop = 3.532 * units::si::meters;

    std::string gga = nmea::gpgga(time, position, quality, hdop);
    EXPECT_EQ("$GPGGA,195559.000000,0020.4000,S,16523.9994,E,2,06,3.5,0.0,M,0.0,M,,*4E", gga);
}

TEST(NMEA, checksum) {
    std::string s = {0, 1, 2, 3, 4, 5};
    auto c = nmea::checksum(s.begin(), s.end());
    EXPECT_EQ(c, 0x01);
}

