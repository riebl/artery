#include "location_table.hpp"
#include <chrono>
#include <limits>

namespace vanetza
{
namespace geonet
{

static_assert(std::numeric_limits<double>::has_quiet_NaN, "quiet NaN value unavailable");

LocationTableEntry::LocationTableEntry(const Runtime& rt) :
    is_neighbour(false), runtime(rt),
    pdr(std::numeric_limits<double>::quiet_NaN()), pdr_update(rt.now())
{
}

StationType LocationTableEntry::station_type() const
{
    return geonet_address().station_type();
}

const Address& LocationTableEntry::geonet_address() const
{
    return position_vector.gn_addr;
}

const MacAddress& LocationTableEntry::link_layer_address() const
{
    return geonet_address().mid();
}

bool LocationTableEntry::is_duplicate_packet(SequenceNumber sn, Timestamp time)
{
    bool is_duplicate = false;

    if (timestamp) {
        if (timestamp.get() > time) {
            is_duplicate = true;
        } else if (timestamp.get() == time) {
            if (!sequence_number) {
                is_duplicate = true;
            } else if (sequence_number.get() >= sn) {
                is_duplicate = true;
            }
        }
    }

    if (!is_duplicate) {
        timestamp = time;
        sequence_number = sn;
    }

    return is_duplicate;
}

bool LocationTableEntry::is_duplicate_packet(Timestamp time)
{
    bool is_duplicate = false;

    if (!timestamp || timestamp.get() <= time) {
        timestamp = time;
    } else {
        is_duplicate = true;
    }

    return is_duplicate;
}

void LocationTableEntry::update_pdr(std::size_t packet_size)
{
    using namespace vanetza::units;

    if (std::isnan(pdr)) {
        pdr = 0.0;
        pdr_update = runtime.now();
    } else {
        const std::chrono::duration<double> time_period = runtime.now() - pdr_update;
        if (time_period.count() > 0.0) {
            double instant_pdr = packet_size / time_period.count();
            pdr *= 0.5;
            pdr += 0.5 * instant_pdr;
            pdr_update = runtime.now();
        }
    }
}

double LocationTableEntry::get_pdr() const
{
    return pdr;
}

LocationTable::LocationTable(const MIB& mib, Runtime& rt) :
    m_table(rt, LocationTableEntryCreator(rt))
{
    m_table.set_lifetime(std::chrono::seconds(mib.itsGnLifetimeLocTE / units::si::seconds));
}

bool LocationTable::has_entry(const Address& addr) const
{
    return m_table.has_value(addr.mid());
}

bool LocationTable::has_neighbours() const
{
    bool found_neighbour = false;
    for (const auto& entry : m_table.map()) {
        if (entry.second.is_neighbour == true) {
            found_neighbour = true;
            break;
        }
    }
    return found_neighbour;
}

auto LocationTable::neighbours() const -> neighbour_range
{
    using namespace boost::adaptors;
    std::function<bool(const typename table_type::value_type&)> filter_fn =
        [](const typename table_type::value_type& v) { return v.second.is_neighbour; };
    return m_table.map() | filtered(filter_fn) | map_values;
}

bool LocationTable::is_duplicate_packet(const Address& addr, SequenceNumber sn, Timestamp t)
{
    return m_table.get_value(addr.mid()).is_duplicate_packet(sn, t);
}

bool LocationTable::is_duplicate_packet(const Address& addr, Timestamp t)
{
    return m_table.get_value(addr.mid()).is_duplicate_packet(t);
}

void LocationTable::update(const LongPositionVector& lpv)
{
    LocationTableEntry* entry = m_table.get_value_ptr(lpv.gn_addr.mid());
    if (entry && !is_empty(entry->position_vector)) {
        if (entry->position_vector.timestamp < lpv.timestamp) {
            entry->position_vector = lpv;
            m_table.refresh(lpv.gn_addr.mid());
        }
    } else {
        m_table.refresh(lpv.gn_addr.mid()).position_vector = lpv;
    }
}

LocationTableEntry& LocationTable::get_entry(const Address& addr)
{
    return m_table.get_value(addr.mid());
}

const LocationTableEntry* LocationTable::get_entry(const Address& addr) const
{
    return m_table.get_value_ptr(addr.mid());
}

const LongPositionVector* LocationTable::get_position(const Address& addr) const
{
    return get_position(addr.mid());
}

const LongPositionVector* LocationTable::get_position(const MacAddress& mac) const
{
    const LongPositionVector* position = nullptr;
    auto* entry = m_table.get_value_ptr(mac);
    if (entry) {
        position = &entry->position_vector;
    }
    return position;
}

} // namespace geonet
} // namespace vanetza

