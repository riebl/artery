#ifndef LOCATION_TABLE_HPP_EMPVZSHQ
#define LOCATION_TABLE_HPP_EMPVZSHQ

#include <vanetza/geonet/address.hpp>
#include <vanetza/geonet/mib.hpp>
#include <vanetza/geonet/position_vector.hpp>
#include <vanetza/geonet/sequence_number.hpp>
#include <vanetza/geonet/station_type.hpp>
#include <vanetza/geonet/timestamp.hpp>
#include <boost/heap/binomial_heap.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/optional.hpp>
#include <boost/range/iterator_range.hpp>
#include <unordered_map>

namespace vanetza
{
namespace geonet
{
namespace detail
{

class LocationTable
{
public:
    struct EntryData;
    class EntryLifetime;
    typedef MacAddress key_type;
    typedef std::greater<EntryLifetime> heap_comparator;
    typedef boost::heap::binomial_heap<EntryLifetime,
            boost::heap::compare<heap_comparator>> heap_type;
    typedef std::unordered_map<key_type, EntryData> map_type;

    class EntryLifetime : public boost::totally_ordered<EntryLifetime>
    {
    public:
        typedef typename Timestamp::duration_type duration_type;

        EntryLifetime() : m_lifetime(0) {}
        EntryLifetime(const key_type&, duration_type lifetime);

        const key_type& key() const { return m_key; }
        void set_lifetime(duration_type ticks) { m_lifetime = ticks; }
        void reduce_lifetime(duration_type);
        bool is_expired() const { return m_lifetime.value() == 0; }

        bool operator<(const EntryLifetime& other) const;
        bool operator==(const EntryLifetime& other) const;

    private:
        key_type m_key;
        duration_type m_lifetime;
    };

    struct EntryData
    {
        EntryData();

        StationType station_type;
        LongPositionVector position;
        bool is_neighbour;
        boost::optional<Timestamp> location_service_expiration;
        boost::optional<Timestamp> received_timestamp;
        boost::optional<SequenceNumber> sequence_number;
        double packet_data_rate;
        Timestamp pdr_update;
        LocationTable::heap_type::handle_type handle;
    };

    LocationTable(const MIB&);

    EntryData& get_entry(const key_type&);
    EntryData* get_entry_ptr(const key_type&);
    const EntryData* get_entry_ptr(const key_type&) const;
    bool has_entry(const key_type&) const;
    void expire(Timestamp::duration_type);
    void refresh(const key_type&);

    map_type::iterator begin() { return m_map.begin(); }
    map_type::iterator end() { return m_map.end(); }
    map_type::const_iterator begin() const { return m_map.begin(); }
    map_type::const_iterator end() const { return m_map.end(); }

private:
    heap_type m_heap;
    map_type m_map;
    const MIB& m_mib;
};

bool is_neighbour(const LocationTable::map_type::value_type&);
const LocationTable::EntryData& get_entry_data(const LocationTable::map_type::value_type&);

} // namespace detail

class LocationTable
{
public:
    typedef typename detail::LocationTable::EntryData entry_type;
    typedef boost::transform_iterator<
                decltype(&detail::get_entry_data),
                boost::filter_iterator<
                    decltype(&detail::is_neighbour),
                    detail::LocationTable::map_type::const_iterator>
            > neighbour_iterator;
    typedef boost::iterator_range<neighbour_iterator> neighbour_range;

    LocationTable(const MIB&);
    bool has_entry(const Address&) const;
    boost::optional<const entry_type&> get_entry(const Address&) const;
    boost::optional<const LongPositionVector&> get_position(const Address&) const;
    boost::optional<const LongPositionVector&> get_position(const MacAddress&) const;
    bool has_neighbours() const;
    neighbour_range neighbours();
    void is_neighbour(const Address&, bool flag);
    bool is_duplicate_packet(const Address& source, SequenceNumber, Timestamp);
    bool is_duplicate_packet(const Address& source, Timestamp);
    void update(const LongPositionVector&);
    void update_pdr(const Address&, std::size_t packet_size, Timestamp);
    double get_pdr(const Address&) const;
    void expire(Timestamp now);

private:
    detail::LocationTable m_table;
    Timestamp m_expiry_update;
};

} // namespace geonet
} // namespace vanetza

#endif /* LOCATION_TABLE_HPP_EMPVZSHQ */

