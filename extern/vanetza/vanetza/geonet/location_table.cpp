#include "location_table.hpp"
#include <vanetza/units/time.hpp>
#include <cmath>
#include <limits>

namespace vanetza
{
namespace geonet
{

static_assert(std::numeric_limits<double>::has_signaling_NaN,
        "signaling NaN value unavailable");


LocationTable::LocationTable(const MIB& mib) : m_table(mib)
{
}

bool LocationTable::has_entry(const Address& addr) const
{
    return m_table.has_entry(addr.mid());
}

bool LocationTable::has_neighbours() const
{
    for (auto iter : m_table) {
        if (iter.second.is_neighbour == true) {
            return true;
        }
    }

    return false;
}

auto LocationTable::neighbours() -> neighbour_range
{
    auto begin = m_table.begin();
    auto end = m_table.end();

    typedef boost::filter_iterator<
        decltype(&detail::is_neighbour),
        decltype(begin)
    > filter_iterator;

    typedef boost::transform_iterator<
        decltype(&detail::get_entry_data),
        filter_iterator
    > transform_iterator;

    filter_iterator filter_begin(&detail::is_neighbour, begin, end);
    filter_iterator filter_end(&detail::is_neighbour, end, end);
    transform_iterator transform_begin(filter_begin, &detail::get_entry_data);
    transform_iterator transform_end(filter_end, &detail::get_entry_data);

    return boost::make_iterator_range(transform_begin, transform_end);
}

bool LocationTable::is_duplicate_packet(const Address& source, SequenceNumber sn, Timestamp time)
{
    entry_type& entry = m_table.get_entry(source.mid());
    bool is_duplicate = false;

    if (entry.received_timestamp) {
        if (entry.received_timestamp.get() > time) {
            is_duplicate = true;
        } else if (entry.received_timestamp.get() == time) {
            if (!entry.sequence_number) {
                is_duplicate = true;
            } else if (entry.sequence_number.get() >= sn) {
                is_duplicate = true;
            }
        }
    }

    if (!is_duplicate) {
        entry.received_timestamp = time;
        entry.sequence_number = sn;
    }

    return is_duplicate;
}

bool LocationTable::is_duplicate_packet(const Address& source, Timestamp time)
{
    entry_type& entry = m_table.get_entry(source.mid());
    bool is_duplicate = false;

    if (!entry.received_timestamp || entry.received_timestamp <= time) {
        entry.received_timestamp = time;
    } else {
        is_duplicate = true;
    }

    return is_duplicate;
}

void LocationTable::update(const LongPositionVector& lpv)
{
    entry_type* entry = m_table.get_entry_ptr(lpv.gn_addr.mid());
    if (entry) {
        if (entry->position.timestamp < lpv.timestamp) {
            entry->position = lpv;
            m_table.refresh(lpv.gn_addr.mid());
        }
    } else {
        m_table.get_entry(lpv.gn_addr.mid()).position = lpv;
    }
}

void LocationTable::update_pdr(const Address& addr, std::size_t packet_size, Timestamp now)
{
    using namespace vanetza::units;

    entry_type& data = m_table.get_entry(addr.mid());
    if (std::isnan(data.packet_data_rate)) {
        data.packet_data_rate = 0.0;
        data.pdr_update = now;
    } else {
        const auto time_period = static_cast<Duration>(now - data.pdr_update);
        if (time_period > 0.0 * si::seconds) {
            const double time_period_s = time_period / si::seconds;
            double instant_pdr = packet_size / time_period_s;
            data.packet_data_rate *= 0.5;
            data.packet_data_rate += 0.5 * instant_pdr;
            data.pdr_update = now;
        }
    }
}

double LocationTable::get_pdr(const Address& addr) const
{
    const entry_type* entry = m_table.get_entry_ptr(addr.mid());
    double pdr = 0.0;
    if (entry) {
        pdr = entry->packet_data_rate;
    }
    return pdr;
}

void LocationTable::is_neighbour(const Address& addr, bool flag)
{
    m_table.get_entry(addr.mid()).is_neighbour = flag;
}

void LocationTable::expire(Timestamp now)
{
    const auto period = now - m_expiry_update;
    m_expiry_update = now;
    m_table.expire(period);
}

boost::optional<const LocationTable::entry_type&>
LocationTable::get_entry(const Address& addr) const
{
    boost::optional<const LocationTable::entry_type&> result;
    auto* entry = m_table.get_entry_ptr(addr.mid());
    if (entry) {
        result = *entry;
    }
    return result;
}

boost::optional<const LongPositionVector&>
LocationTable::get_position(const Address& addr) const
{
    return get_position(addr.mid());
}

boost::optional<const LongPositionVector&>
LocationTable::get_position(const MacAddress& mac) const
{
    boost::optional<const LongPositionVector&> position;
    auto* entry = m_table.get_entry_ptr(mac);
    if (entry) {
        position = entry->position;
    }

    return position;
}


namespace detail {

LocationTable::LocationTable(const MIB& mib) : m_mib(mib)
{
}

auto LocationTable::get_entry(const key_type& key) -> EntryData&
{
    EntryData* entry = get_entry_ptr(key);
    if (!entry) {
        entry = &m_map[key];
        EntryLifetime lifetime(key, Timestamp::duration_type(m_mib.itsGnLifetimeLocTE));
        heap_type::handle_type handle = m_heap.push(std::move(lifetime));
        entry->handle = handle;
    }

    assert(entry);
    return *entry;
}

auto LocationTable::get_entry_ptr(const key_type& key) -> EntryData*
{
    EntryData* ptr = nullptr;
    auto it = m_map.find(key);
    if (it != m_map.end()) {
        ptr = &it->second;
    }

    return ptr;
}

auto LocationTable::get_entry_ptr(const key_type& key) const -> const EntryData*
{
    const EntryData* ptr = nullptr;
    auto it = m_map.find(key);
    if (it != m_map.end()) {
        ptr = &it->second;
    }

    return ptr;
}

bool LocationTable::has_entry(const key_type& key) const
{
    auto it = m_map.find(key);
    return (it != m_map.end());
}

void LocationTable::expire(EntryLifetime::duration_type ticks)
{
    // age all entries
    for (auto it : m_map) {
        heap_type::handle_type handle = it.second.handle;
        (*handle).reduce_lifetime(ticks);
        m_heap.update(handle);
    }

    // delete all entries with lifetime <= 0
    while (!m_heap.empty() && m_heap.top().is_expired()) {
        m_map.erase(m_heap.top().key());
        m_heap.pop();
    }
}

void LocationTable::refresh(const key_type& key)
{
    auto& entry = get_entry(key);
    heap_type::handle_type handle = entry.handle;
    (*handle).set_lifetime(Timestamp::duration_type(m_mib.itsGnLifetimeLocTE));
    m_heap.update(handle);
}

LocationTable::EntryData::EntryData() :
    station_type(StationType::UNKNOWN), is_neighbour(false),
    packet_data_rate(std::numeric_limits<double>::signaling_NaN())
{
}

LocationTable::EntryLifetime::EntryLifetime(const key_type& key, duration_type lifetime) :
    m_key(key), m_lifetime(lifetime)
{
}

void LocationTable::EntryLifetime::reduce_lifetime(duration_type ticks)
{
    m_lifetime -= std::min(m_lifetime, ticks);
}

bool LocationTable::EntryLifetime::operator<(const EntryLifetime& other) const
{
    return (this->m_lifetime < other.m_lifetime);
}

bool LocationTable::EntryLifetime::operator==(const EntryLifetime& other) const
{
    return (this->m_lifetime == other.m_lifetime);
}

bool is_neighbour(const LocationTable::map_type::value_type& kv_pair)
{
    return kv_pair.second.is_neighbour;
}

const LocationTable::EntryData&
get_entry_data(const LocationTable::map_type::value_type& kv_pair)
{
    return kv_pair.second;
}

} // namespace detail
} // namespace geonet
} // namespace vanetza

