/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/den/Memory.h"
#include "artery/application/Timer.h"
#include <omnetpp/csimulation.h>

using omnetpp::SimTime;

namespace artery
{
namespace den
{

ActionID::ActionID(const ActionID_t& asn1) :
    station_id(asn1.originatingStationID),
    sequence_number(asn1.sequenceNumber)
{
}

bool operator<(const ActionID& lhs, const ActionID& rhs)
{
    if (lhs.station_id == rhs.station_id) {
        return lhs.sequence_number < rhs.sequence_number;
    } else {
        return lhs.station_id < rhs.station_id;
    }
}

bool operator==(const ActionID& lhs, const ActionID& rhs)
{
    return lhs.station_id == rhs.station_id && lhs.sequence_number == rhs.sequence_number;
}

bool operator<(const INTEGER_t& lhs, const INTEGER_t& rhs)
{
    // TODO this might fail because long is not wide enough (asn1c limitation)
    long lhs_value = 0, rhs_value = 0;
    if (asn_INTEGER2long(&lhs, &lhs_value) != 0) {
        throw std::range_error("lhs value cannot be converted to long");
    }
    if (asn_INTEGER2long(&rhs, &rhs_value) != 0) {
        throw std::range_error("rhs value cannot be converted to long");
    }

    return lhs_value < rhs_value;
}

Reception::Reception(const DenmObject& object) :
    timestamp(omnetpp::simTime()),
    message(object.shared_ptr())
{
}

vanetza::Clock::time_point Reception::expiry() const
{
    unsigned long detectionTimeRaw = 0;
    const ManagementContainer_t& denmManagement = (*message)->denm.management;
    if (asn_INTEGER2ulong(&denmManagement.detectionTime, &detectionTimeRaw) != 0) {
        throw std::range_error("DENM detectionTime cannot be converted to unsigned long");
    }
    vanetza::Clock::time_point detectionTime { std::chrono::milliseconds(detectionTimeRaw) };

    vanetza::Clock::duration validityDuration = std::chrono::seconds(600);
    if (denmManagement.validityDuration) {
        validityDuration = std::chrono::seconds(*denmManagement.validityDuration / ValidityDuration_oneSecondAfterDetection);
    }

    return detectionTime + validityDuration;
}

ActionID Reception::action_id() const
{
    return ActionID((*message)->denm.management.actionID);
}

CauseCode Reception::cause_code() const
{
    const SituationContainer* situation = (*message)->denm.situation;
    if (situation) {
        return convert(situation->eventType.causeCode);
    } else {
        return static_cast<CauseCode>(0);
    }
}

Memory::Memory(const Timer& timer) :
    m_timer(timer)
{
}

void Memory::received(const DenmObject& denm)
{
    // TODO handle termination DENMs
    ActionID action_id { denm.asn1()->denm.management.actionID };
    auto& idx_action_id = m_container.get<by_action_id>();
    auto found = idx_action_id.find(action_id);
    if (found == idx_action_id.end()) {
        m_container.insert(den::Reception {denm});
    } else {
        const ManagementContainer_t& stored = (*found->message)->denm.management;
        const ManagementContainer_t& received = denm.asn1()->denm.management;
        if (stored.referenceTime < received.referenceTime) {
            idx_action_id.replace(found, den::Reception {denm});
        }
    }
}

void Memory::drop()
{
    auto& idx_expiry = m_container.get<by_expiry>();
    auto first_not_less = idx_expiry.lower_bound(m_timer.getCurrentTime());
    idx_expiry.erase(idx_expiry.begin(), first_not_less);
}

unsigned Memory::count(CauseCode cause_code) const
{
    auto& idx_cause_code = m_container.get<by_cause_code>();
    return idx_cause_code.count(cause_code);
}

auto Memory::messages(CauseCode cause_code) const -> boost::iterator_range<cause_code_iterator>
{
    auto& idx_cause_code = m_container.get<by_cause_code>();
    auto equal_cause_code = idx_cause_code.equal_range(cause_code);
    return boost::make_iterator_range(equal_cause_code);
}

} // namespace den
} // namespace artery
