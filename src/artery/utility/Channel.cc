#include "Channel.h"
#include <boost/lexical_cast.hpp>
#include <omnetpp/cexception.h>
#include <limits>
#include <regex>

namespace artery
{

ChannelNumber parseChannelNumber(const std::string& str)
{
    static const std::regex number_regex("[0-9]+");

    ChannelNumber channel = 0;

    if (std::regex_match(str, number_regex)) {
        channel = boost::lexical_cast<ChannelNumber>(str);
    } else if (str == "CCH" || str == "SCH0") {
        channel = channel::CCH;
    } else if (str == "SCH1") {
        channel = channel::SCH1;
    } else if (str == "SCH2") {
        channel = channel::SCH2;
    } else if (str == "SCH3") {
        channel = channel::SCH3;
    } else if (str == "SCH4") {
        channel = channel::SCH4;
    } else if (str == "SCH5") {
        channel = channel::SCH5;
    } else if (str == "SCH6") {
        channel = channel::SCH6;
    }

    return channel;
}

unsigned getChannelRank(ChannelNumber ch)
{
    switch (ch) {
        case channel::CCH:
            return 0;
        case channel::SCH1:
            return 1;
        case channel::SCH2:
            return 2;
        case channel::SCH3:
            return 3;
        case channel::SCH4:
            return 4;
        case channel::SCH5:
            return 5;
        case channel::SCH6:
            return 6;
        default:
            return std::numeric_limits<unsigned>::max();
    }
}

} // namespace artery
