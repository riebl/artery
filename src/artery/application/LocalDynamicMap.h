#ifndef LOCALDYNAMICMAP_H_AL7SS9KT
#define LOCALDYNAMICMAP_H_AL7SS9KT

#include "artery/application/CaObject.h"
#include <omnetpp/simtime.h>
#include <vanetza/asn1/cam.hpp>
#include <cstdint>
#include <functional>
#include <map>

class Timer;

namespace artery
{

class LocalDynamicMap
{
public:
    using StationID = uint32_t;
    using Cam = vanetza::asn1::Cam;
    using CamPredicate = std::function<bool(const Cam&)>;

    LocalDynamicMap(const Timer&);
    void updateAwareness(const CaObject&);
    void dropExpired();
    unsigned count(const CamPredicate&) const;

private:
    struct AwarenessEntry
    {
        AwarenessEntry(const CaObject&, omnetpp::SimTime);
        AwarenessEntry(AwarenessEntry&&) = default;
        AwarenessEntry& operator=(AwarenessEntry&&) = default;

        omnetpp::SimTime expiry;
        CaObject object;
    };

    const Timer& mTimer;
    std::map<StationID, AwarenessEntry> mCaMessages;
};

} // namespace artery

#endif /* LOCALDYNAMICMAP_H_AL7SS9KT */

