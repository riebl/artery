#ifndef SAMPLING_H_3WCIABPU
#define SAMPLING_H_3WCIABPU

#include "artery/application/SampleBuffer.h"
#include <omnetpp/simtime.h>
#include <algorithm>
#include <stdexcept>

template<typename IN, typename OUT = IN>
class Sampler
{
public:
    using input_type = IN;
    using output_type = OUT;
    using buffer_type = SampleBuffer<output_type>;

    const buffer_type& buffer() const { return m_buffer; }

protected:
    buffer_type m_buffer;
};

template<typename IN, typename OUT = IN>
class IntervalSampler : public Sampler<IN, OUT>
{
public:
    using base_type = Sampler<IN, OUT>;

    class undersampling_error : public std::runtime_error {};
    class oversampling_error : public std::runtime_error {};

    void setDuration(omnetpp::SimTime d)
    {
        m_duration = std::max(d, omnetpp::SimTime::ZERO);
        setCapacity();
    }

    omnetpp::SimTime getDuration() const { return m_duration; }

    void setInterval(omnetpp::SimTime interval)
    {
        if (m_interval != interval) {
            base_type::m_buffer.clear();
        }
        m_interval = std::max(interval, omnetpp::SimTime::ZERO);
        setCapacity();
    }

    omnetpp::SimTime getInterval() const { return m_interval; }

    omnetpp::SimTime offset(omnetpp::SimTime now) const
    {
        auto offset = omnetpp::SimTime::ZERO;
        if (!base_type::m_buffer.empty()) {
            const auto& latest_sample = base_type::m_buffer.latest();
            offset = now - (latest_sample.timestamp + m_interval);
        }
        return offset;
    }

private:
    void setCapacity()
    {
        std::size_t samples = 0;
        if (!m_interval.isZero()) {
            samples = std::ceil(m_duration / m_interval);
        }
        base_type::m_buffer.set_capacity(samples);
    }

    omnetpp::SimTime m_interval;
    omnetpp::SimTime m_duration;
};

template<typename T>
class StrictSampler : public IntervalSampler<T>
{
public:
    using base_type = IntervalSampler<T>;

    void feed(const typename base_type::input_type& sample, omnetpp::SimTime timestamp)
    {
        auto offset = base_type::offset(timestamp);
        if (offset == omnetpp::SimTime::ZERO) {
            base_type::m_buffer.insert(sample, timestamp);
        } else if (offset < omnetpp::SimTime::ZERO) {
            throw base_type::oversampling_error();
        } else {
            throw base_type::undersampling_error();
        }
    }
};

template<typename T>
class SkipEarlySampler : public IntervalSampler<T>
{
public:
    using base_type = IntervalSampler<T>;

    void feed(const typename base_type::input_type& sample, omnetpp::SimTime timestamp)
    {
        auto offset = base_type::offset(timestamp);
        if (offset >= omnetpp::SimTime::ZERO) {
            base_type::m_buffer.insert(sample, timestamp);
        }
    }
};

#endif /* SAMPLING_H_3WCIABPU */

