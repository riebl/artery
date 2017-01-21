#ifndef LOCATION_TABLE_HPP_EMPVZSHQ
#define LOCATION_TABLE_HPP_EMPVZSHQ

#include <vanetza/geonet/address.hpp>
#include <vanetza/geonet/mib.hpp>
#include <vanetza/geonet/position_vector.hpp>
#include <vanetza/geonet/sequence_number.hpp>
#include <vanetza/geonet/soft_state_map.hpp>
#include <vanetza/geonet/station_type.hpp>
#include <vanetza/geonet/timestamp.hpp>
#include <boost/optional.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/map.hpp>

namespace vanetza
{
namespace geonet
{

class LocationTableEntry
{
public:
    LocationTableEntry(const Runtime& rt);

    const Address& geonet_address() const;
    const MacAddress& link_layer_address() const;
    StationType station_type() const;

    /**
     * Check if packet is a duplicate based on its sequence number and time stamp.
     * This implements algorithm A.2 in EN 302 636-4-1.
     * \param sn sequence number
     * \param t time stamp
     * \return true if duplicate
     */
    bool is_duplicate_packet(SequenceNumber, Timestamp);

    /**
     * Check if packet is a duplicate based on its time stamp.
     * This implements algorithm A.3 in EN 302 636-4-1.
     * \param t time stamp
     * \return true if duplicate
     */
    bool is_duplicate_packet(Timestamp);

    /**
     * Get packed data rate (PDR) of corresponding source.
     * \return PDR in bytes per second, might be not-a-number
     */
    double get_pdr() const;

    /**
     * Update packet data rate.
     * See Annex B of EN 302 636-4-1 for details.
     * \param packet_size received number of bytes
     */
    void update_pdr(std::size_t packet_size);

    bool is_neighbour;
    LongPositionVector position_vector;

private:
    const Runtime& runtime;
    boost::optional<Timestamp> timestamp;
    boost::optional<SequenceNumber> sequence_number;
    double pdr;
    Clock::time_point pdr_update;
};

class LocationTableEntryCreator
{
public:
    LocationTableEntryCreator(const Runtime& rt) : m_runtime(rt) {}
    LocationTableEntry operator()() { return LocationTableEntry(m_runtime); }

private:
    const Runtime& m_runtime;
};

/**
 * GeoNetworking LocationTable
 * See section 7.1 of EN 302 636-4-1 for details.
 */
class LocationTable
{
public:
    using table_type = SoftStateMap<MacAddress, LocationTableEntry, LocationTableEntryCreator>;
    using neighbour_range =
        boost::select_second_const_range<
            boost::filtered_range<
                std::function<bool(const typename table_type::value_type&)>,
                const typename table_type::map_range>>;

    LocationTable(const MIB&, Runtime&);
    bool has_entry(const Address&) const;
    LocationTableEntry& get_entry(const Address&);
    const LocationTableEntry* get_entry(const Address&) const;
    const LongPositionVector* get_position(const Address&) const;
    const LongPositionVector* get_position(const MacAddress&) const;
    bool has_neighbours() const;
    neighbour_range neighbours() const;
    bool is_duplicate_packet(const Address& source, SequenceNumber, Timestamp);
    bool is_duplicate_packet(const Address& source, Timestamp);
    void update(const LongPositionVector&);
    void drop_expired() { m_table.drop_expired(); }

private:
    table_type m_table;
};

} // namespace geonet
} // namespace vanetza

#endif /* LOCATION_TABLE_HPP_EMPVZSHQ */

