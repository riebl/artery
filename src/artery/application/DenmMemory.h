/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_DENM_MEMORY_H_NNPEDDM9
#define ARTERY_DENM_MEMORY_H_NNPEDDM9

#include "artery/application/DenmObject.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <omnetpp/simtime.h>
#include <vanetza/asn1/denm.hpp>
#include <vanetza/common/clock.hpp>
#include <memory>

class Timer;

namespace artery
{
namespace denm
{

struct ActionID
{
    ActionID(const ActionID_t&);

    uint32_t station_id;
    uint16_t sequence_number;
};

bool operator<(const ActionID&, const ActionID&);
bool operator==(const ActionID&, const ActionID&);

struct Reception
{
    Reception(const DenmObject&);

    omnetpp::SimTime timestamp;
    std::shared_ptr<const vanetza::asn1::Denm> message;

    vanetza::Clock::time_point expiry() const;
    ActionID action_id() const;
    CauseCode cause_code() const;
};

class Memory
{
    struct by_action_id {};
    struct by_expiry {};
    struct by_cause_code {};

    using container_type = boost::multi_index_container<Reception,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<by_action_id>,
                boost::multi_index::const_mem_fun<Reception, ActionID, &Reception::action_id>>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<by_expiry>,
                boost::multi_index::const_mem_fun<Reception, vanetza::Clock::time_point, &Reception::expiry>>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<by_cause_code>,
                boost::multi_index::const_mem_fun<Reception, CauseCode, &Reception::cause_code>>
        >>;
public:
    using cause_code_iterator = decltype(container_type().get<by_cause_code>().begin());

    Memory(const Timer&);

    void received(const DenmObject&);
    void drop();
    unsigned count(CauseCode) const;
    boost::iterator_range<cause_code_iterator> messages(CauseCode) const;

private:
    const Timer& m_timer;
    container_type m_container;
};

} // namespace denm
} // namespace artery

#endif /* ARTERY_DENM_MEMORY_H_NNPEDDM9 */

