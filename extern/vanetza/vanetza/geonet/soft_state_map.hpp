#ifndef SOFT_STATE_MAP_HPP_B0MARRWZ
#define SOFT_STATE_MAP_HPP_B0MARRWZ

#include <vanetza/common/clock.hpp>
#include <vanetza/common/runtime.hpp>
#include <boost/heap/binomial_heap.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <cassert>
#include <functional>
#include <type_traits>
#include <unordered_map>

namespace vanetza
{
namespace geonet
{

template<typename VALUE>
struct SoftStateDefaultCreator
{
    VALUE operator()() { return VALUE(); }
};

/**
 * SoftStateMap is a map data structure with expiring entries
 * \tparam KEY key type
 * \tparam VALUE mapped type
 * \tparam CTOR optional creator of values
 */
template<typename KEY, typename VALUE, typename CTOR = SoftStateDefaultCreator<VALUE>>
class SoftStateMap
{
public:
    using key_type = KEY;
    using mapped_type = VALUE;
    using value_type = std::pair<const key_type&, mapped_type&>;
    using creator_type = CTOR;

private:
    class ExpiryWithKey : public Clock::time_point
    {
    public:
        ExpiryWithKey() = default;
        ExpiryWithKey(const key_type& key, Clock::time_point expiry) :
            Clock::time_point(expiry), m_key(key) {}

        const key_type& key() const { return m_key; }

    private:
        key_type m_key;
    };
    using heap_type = boost::heap::binomial_heap<ExpiryWithKey, boost::heap::compare<std::greater<ExpiryWithKey>>>;

    struct ValueWithHandle
    {
        ValueWithHandle(mapped_type&& v) : value(std::move(v)) {}

        typename heap_type::handle_type handle;
        mapped_type value;

        mapped_type& operator*() { return value; }
        const mapped_type& operator*() const { return value; }
    };
    using map_type = std::unordered_map<key_type, ValueWithHandle>;

    using data_range = boost::iterator_range<typename map_type::iterator>;
    using data_filter = std::function<bool(const typename map_type::value_type&)>;
    using data_transform = std::function<value_type(typename map_type::value_type&)>;

public:
    /**
     * Construct SoftStateMap
     * \param rt runtime object
     * \note This constructor is only available if CTOR is default constructible
     */
    template<typename T = CTOR>
    SoftStateMap(const Runtime& rt, typename std::enable_if<std::is_default_constructible<T>::value>::type* = nullptr) :
        m_runtime(rt)
    {
    }

    /**
     * Construct SoftStateMap
     * \param rt runtime object
     * \param ctor value creator
     */
    SoftStateMap(const Runtime& rt, creator_type&& ctor) :
        m_runtime(rt), m_creator(std::move(ctor))
    {
    }

    /**
     * Set lifetime duration used for new and refreshed entries
     * \param lifetime entry lieftime
     */
    void set_lifetime(Clock::duration lifetime)
    {
        m_lifetime = lifetime;
    }

    /**
     * Get value mapped to key
     * \param key
     * \return existing value entry or just created entry
     */
    mapped_type& get_value(const key_type& key)
    {
        return get_data(key).value;
    }

    /**
     * Get non-expired value pointer mapped to key
     * \param key
     * \return pointer to value or nullptr if not existing
     */
    mapped_type* get_value_ptr(const key_type& key)
    {
        auto* data = get_data_ptr(key);
        return data && !is_expired(*data->handle) ? &data->value : nullptr;
    }

    /**
     * Get non-expired value pointer mapped to key
     * \param key
     * \return pointer to value or nullptr if not existing
     */
    const mapped_type* get_value_ptr(const key_type& key) const
    {
        auto* data = get_data_ptr(key);
        return data && !is_expired(*data->handle) ? &data->value : nullptr;
    }

    /**
     * Check if non-expired value for given key exists
     * \param key
     * \return true if entry exists
     */
    bool has_value(const key_type& key) const
    {
        return get_value_ptr(key) != nullptr;
    }

    /**
     * Refresh lifetime of entry associated with given key
     * \param key
     * \return associated value (might have been created)
     */
    mapped_type& refresh(const key_type& key)
    {
        auto* data = get_data_ptr(key);
        if (data) {
            refresh(*data->handle);
        } else {
            data = &get_data(key);
            assert(data != nullptr);
        }
        return data->value;
    }

    /**
     * Drop all entries with expired lifetime.
     * Expired but still stored entries are only hided at retrieval until calling this method.
     */
    void drop_expired()
    {
        while (!m_heap.empty() && is_expired(m_heap.top())) {
            m_map.erase(m_heap.top().key());
            m_heap.pop();
        }
    }

    using map_range = boost::transformed_range<data_transform, const boost::filtered_range<data_filter, data_range>>;

    /**
     * Create a range of all non-expired entries mimicking STL's map interface
     */
    map_range map()
    {
        data_filter filter_fn = [this](const typename map_type::value_type& v) {
            return !this->is_expired(*v.second.handle);
        };
        data_transform transform_fn = [](typename map_type::value_type& v) {
            return value_type { v.first, v.second.value };
        };
        using namespace boost::adaptors;
        data_range range_all = boost::make_iterator_range(m_map.begin(), m_map.end());
        return range_all | filtered(filter_fn) | transformed(transform_fn);
    }

    const map_range map() const
    {
        return const_cast<SoftStateMap*>(this)->map();
    }

private:
    ValueWithHandle& get_data(const key_type& key)
    {
        auto* data = get_data_ptr(key);
        if (!data) {
            auto insertion = m_map.emplace(std::piecewise_construct,
                    std::forward_as_tuple(key), std::forward_as_tuple(m_creator()));
            data = &insertion.first->second;
            data->handle = m_heap.push(ExpiryWithKey {key, m_runtime.now() + m_lifetime});
        } else if (is_expired(*data->handle)) {
            // resurrect this data element, i.e. pretend it has just been created
            refresh(*data->handle);
        }
        return *data;
    }

    ValueWithHandle* get_data_ptr(const key_type& key)
    {
        auto it = m_map.find(key);
        return it != m_map.end() ? &it->second : nullptr;
    }

    const ValueWithHandle* get_data_ptr(const key_type& key) const
    {
        auto it = m_map.find(key);
        return it != m_map.end() ? &it->second : nullptr;
    }

    bool is_expired(const ExpiryWithKey& expiry) const
    {
        return m_runtime.now() > expiry;
    }

    void refresh(ExpiryWithKey& expiry)
    {
        static_cast<Clock::time_point&>(expiry) = m_runtime.now() + m_lifetime;
    }

    const Runtime& m_runtime;
    Clock::duration m_lifetime;
    creator_type m_creator;
    heap_type m_heap;
    map_type m_map;
};

} // namespace geonet
} // namespace vanetza

#endif /* SOFT_STATE_MAP_HPP_B0MARRWZ */
