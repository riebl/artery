#ifndef ARTERY_SAMPLEBUFFER_H_QHFTL7Z2
#define ARTERY_SAMPLEBUFFER_H_QHFTL7Z2

#include <boost/circular_buffer.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <omnetpp/simtime.h>
#include <iterator>
#include <stdexcept>

template<typename T>
struct Sample
{
    using value_type = T;

    Sample(const T& v, omnetpp::SimTime t) :
         value(v), timestamp(t) {}
    Sample(T&& v, omnetpp::SimTime t) :
         value(std::move(v)), timestamp(t) {}

    T value;
    omnetpp::SimTime timestamp;
};

template<typename T>
class SampleBuffer
{
public:
    using sample_type = T;
    using value_type = Sample<T>;
    using buffer_type = boost::circular_buffer<value_type>;
    using iterator = typename buffer_type::const_iterator;

    struct range : public boost::iterator_range<iterator>
    {
        range(iterator begin, iterator end) :
            boost::iterator_range<iterator>(begin, end)
        {
        }

        omnetpp::SimTime duration() const
        {
            auto d = omnetpp::SimTime::ZERO;
            if (range::size() >= 2) {
                d = range::begin()->timestamp - std::prev(range::end())->timestamp;
            }
            return d;
        }
    };

    void set_capacity(std::size_t n) { m_buffer.set_capacity(n); }
    std::size_t size() const { return m_buffer.size(); }
    bool empty() const { return m_buffer.empty(); }
    bool full() const { return m_buffer.full(); }
    iterator begin() const { return iterator(m_buffer.begin()); }
    iterator end() const { return iterator(m_buffer.end()); }

    const value_type& latest() const { return m_buffer.front(); }

    range not_before(omnetpp::SimTime limit) const
    {
        iterator begin = m_buffer.begin();
        iterator end = std::lower_bound(m_buffer.begin(), m_buffer.end(), limit,
                [](const value_type& sample, const omnetpp::SimTime& limit) { return sample.timestamp > limit; });
        return range(begin, end);
    }

    void insert(const sample_type& sample, omnetpp::SimTime timestamp)
    {
        insert_impl(sample, timestamp);
    }

    void insert(sample_type&& sample, omnetpp::SimTime timestamp)
    {
        insert_impl(std::move(sample), timestamp);
    }

    void clear() { m_buffer.clear(); }

private:
    template<typename S>
    void insert_impl(S&& sample, omnetpp::SimTime timestamp)
    {
        if (m_buffer.empty() || m_buffer.front().timestamp < timestamp) {
            m_buffer.push_front(value_type { std::forward<S>(sample), timestamp });
        } else {
            throw std::logic_error("chronological order of samples violated");
        }
    }

    buffer_type m_buffer;
};

#endif /* ARTERY_SAMPLEBUFFER_H_QHFTL7Z2 */

