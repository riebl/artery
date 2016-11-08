#ifndef LRU_CACHE_HPP_CII58WXX
#define LRU_CACHE_HPP_CII58WXX

#include <cassert>
#include <functional>
#include <list>
#include <unordered_map>

namespace vanetza
{

/**
 * \brief Least-Recently-Used cache
 *
 * If an entry is accessed which is not yet cached, it will created
 * by invocation of the generator function.
 *
 * \tparam KEY key type
 * \tparam VALUE value_type, i.e. type of cache entries
 * \tparam GENERATOR generator function for creating values from key
 */
template<typename KEY, typename VALUE, typename GENERATOR = std::function<VALUE(const KEY&)>>
class LruCache
{
public:
    using generator = GENERATOR;
    using key_type = KEY;
    using value_type = VALUE;

    /**
     * \param g user-defined generator function
     * \param capacity maximum number of cache entries
     */
    LruCache(generator g, std::size_t capacity) :
        m_generator(g),
        m_capacity(capacity)
    {
    }

    /**
     * \brief Access a cache entry
     *
     * If cache entry does not yet exist it will be created.
     * The least recently accessed entry might get dropped.
     *
     * \param key identifier of cache entry
     * \return cached value
     */
    value_type& operator[](const key_type& key)
    {
        auto found = m_cache.find(key);
        if (found == m_cache.end()) {
            return add(key);
        } else {
            return refresh(found);
        }
    }

private:
    using list_type = std::list<key_type>;
    using entry_type = std::pair<value_type, typename list_type::iterator>;
    using map_type = std::unordered_map<key_type, entry_type>;

    value_type& add(const key_type& k)
    {
        if (m_cache.size() >= m_capacity) {
            remove();
        }

        m_index.emplace_front(k);
        entry_type entry = std::make_pair(m_generator(k), m_index.begin());
        auto insertion = m_cache.insert(std::make_pair(k, std::move(entry)));
        assert(insertion.second == true);
        return insertion.first->second.first;
    }

    value_type& refresh(typename map_type::iterator found)
    {
        m_index.splice(m_index.begin(), m_index, found->second.second);
        assert(m_index.begin() == found->second.second);
        return found->second.first;
    }

    void remove()
    {
        if (!m_index.empty()) {
            m_cache.erase(m_index.back());
            m_index.pop_back();
        }
    }

    generator m_generator;
    std::size_t m_capacity;
    list_type m_index;
    map_type m_cache;
};

} // namespace vanetza

#endif /* LRU_CACHE_HPP_CII58WXX */

