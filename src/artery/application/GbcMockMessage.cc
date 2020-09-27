#include "GbcMockMessage.h"
#include <inet/common/geometry/common/Coord.h>
#include <omnetpp.h>
#include <set>

namespace artery
{

Register_Class(GbcMockMessage)

class GbcLatencyResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto gbc = dynamic_cast<GbcMockMessage*>(object)) {
            const auto latency = t - gbc->getGenerationTimestamp();
            fire(this, t, latency, details);
        }
    }
};

Register_ResultFilter("gbcLatency", GbcLatencyResultFilter)


class GbcRangeResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto gbc = dynamic_cast<GbcMockMessage*>(object)) {
            auto rx = omnetpp::check_and_cast<inet::Coord*>(details);
            inet::Coord tx { gbc->getSourcePosition().x.value(), gbc->getSourcePosition().y.value() };
            const auto range = tx.distance(*rx);
            fire(this, t, range, details);
        }
    }
};

Register_ResultFilter("gbcRange", GbcRangeResultFilter)


class GbcUniqueReceptionResultFilter : public cObjectResultFilter
{
protected:
    using Identifier = std::tuple<int, SimTime>;

    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        auto msg = check_and_cast<GbcMockMessage*>(object);
        auto insert = mIdentifiers.emplace(msg->getSourceStation(), msg->getGenerationTimestamp());
        if (insert.second) {
            fire(this, t, msg, details);
        }
    }

private:
    std::set<Identifier> mIdentifiers;
};

Register_ResultFilter("gbcUniqueReception", GbcUniqueReceptionResultFilter)

} // namespace artery
