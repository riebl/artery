/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_CHANNEL_LOAD_SAMPLER_H_NFJLZK0H
#define ARTERY_CHANNEL_LOAD_SAMPLER_H_NFJLZK0H

#include <omnetpp/simtime.h>
#include <deque>
#include <ostream>
#include <tuple>

namespace artery
{

class ChannelLoadSampler
{
    public:
        ChannelLoadSampler();
        void reset();
        void busy(bool flag);
        double cbr();

        friend std::ostream& operator<<(std::ostream& os, const ChannelLoadSampler&);

    private:
        void updateCbr();
        unsigned computePendingSamples() const;

        omnetpp::SimTime mLastUpdate;
        std::deque<std::tuple<unsigned, bool>> mSamples;
        bool mBusy;
        double mCbr;
};

} // namespace artery

#endif /* ARTERY_CHANNEL_LOAD_SAMPLER_H_NFJLZK0H */
