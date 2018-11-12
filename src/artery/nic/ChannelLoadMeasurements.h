#ifndef CHANNELLOADMEASUREMENTS_H_
#define CHANNELLOADMEASUREMENTS_H_

#include <omnetpp/simtime.h>
#include <vanetza/dcc/channel_load.hpp>
#include <boost/circular_buffer.hpp>

class ChannelLoadMeasurements
{
    public:
        ChannelLoadMeasurements();
        void reset();
        void busy();
        void idle();
        vanetza::dcc::ChannelLoad channel_load();

    private:
        void fill(bool busy);

        boost::circular_buffer<bool> m_samples;
        omnetpp::SimTime m_last_update;
        bool m_busy;
};

#endif /* CHANNELLOADMEASUREMENTS_H_ */
