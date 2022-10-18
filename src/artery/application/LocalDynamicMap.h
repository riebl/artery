#ifndef ARTERY_LOCALDYNAMICMAP_H_AL7SS9KT
#define ARTERY_LOCALDYNAMICMAP_H_AL7SS9KT

#include "artery/application/CaObject.h"
#include <omnetpp/simtime.h>
#include <vanetza/asn1/cam.hpp>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>

namespace artery
{

class Timer;

class LocalDynamicMap
{
public:
    using StationID = uint32_t;
    using Cam = vanetza::asn1::Cam;
    using CamPredicate = std::function<bool(const Cam&)>;

    class AwarenessEntry
    {
    public:
        AwarenessEntry(const CaObject&, omnetpp::SimTime);
        AwarenessEntry(AwarenessEntry&&) = default;
        AwarenessEntry& operator=(AwarenessEntry&&) = default;

        omnetpp::SimTime expiry() const { return mExpiry; }
        const Cam& cam() const { return mObject.asn1(); }
        std::shared_ptr<const Cam> camPtr() const { return mObject.shared_ptr(); }

    private:
        omnetpp::SimTime mExpiry;
        CaObject mObject;
    };

    using AwarenessEntries = std::map<StationID, AwarenessEntry>;

    LocalDynamicMap(const Timer&);
    void updateAwareness(const CaObject&);
    void dropExpired();
    unsigned count(const CamPredicate&) const;
    std::shared_ptr<const Cam> getCam(StationID) const;
    const AwarenessEntries& allEntries() const { return mCaMessages; }

private:
    const Timer& mTimer;
    AwarenessEntries mCaMessages;
};

} // namespace artery

#endif /* ARTERY_LOCALDYNAMICMAP_H_AL7SS9KT */

