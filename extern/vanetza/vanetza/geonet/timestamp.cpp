#include "timestamp.hpp"
#include "vanetza/common/byte_order.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <cassert>
#include <chrono>
#include <limits>

namespace vanetza
{
namespace geonet
{

Timestamp::unit_type Timestamp::millisecond()
{
    return Timestamp::unit_type();
}

boost::posix_time::ptime Timestamp::start_time()
{
    static const boost::posix_time::ptime start {
        boost::gregorian::date(2004, 1, 1),
        boost::posix_time::milliseconds(0)
    };
    return start;
}

Timestamp::Timestamp(const Clock::time_point& time)
{
    using namespace std::chrono;
    const auto since_epoch = time.time_since_epoch();
    const auto since_epoch_ms = duration_cast<milliseconds>(since_epoch);
    m_timestamp = since_epoch_ms.count() * absolute_unit_type();
}

Timestamp::Timestamp(const boost::posix_time::ptime& time)
{
    assert(time >= start_time());
    const value_type since_start_ms = (time - start_time()).total_milliseconds();
    m_timestamp = since_start_ms * absolute_unit_type();
}

Timestamp& Timestamp::operator+=(duration_type rhs)
{
    m_timestamp += rhs;
    return *this;
}

Timestamp& Timestamp::operator-=(duration_type rhs)
{
    m_timestamp -= rhs;
    return *this;
}

Timestamp operator+(Timestamp lhs, Timestamp::duration_type rhs)
{
    lhs += rhs;
    return lhs;
}

Timestamp operator-(Timestamp lhs, Timestamp::duration_type rhs)
{
    lhs -= rhs;
    return lhs;
}

Timestamp::duration_type operator-(Timestamp lhs, Timestamp rhs)
{
    return lhs.quantity() - rhs.quantity();
}

bool Timestamp::before(const Timestamp& other) const
{
    return *this < other;
}

bool Timestamp::after(const Timestamp& other) const
{
    return *this > other;
}

bool is_greater(Timestamp lhs, Timestamp rhs)
{
    const auto max = std::numeric_limits<Timestamp::value_type>::max();
    const Timestamp::value_type lhs_raw = lhs.raw();
    const Timestamp::value_type rhs_raw = rhs.raw();

    if ((lhs_raw > rhs_raw && lhs_raw - rhs_raw <= max/2) ||
        (rhs_raw > lhs_raw && rhs_raw - lhs_raw > max/2)) {
        return true;
    } else {
        return false;
    }
}

bool operator<(Timestamp lhs, Timestamp rhs)
{
    return (!is_greater(lhs, rhs) && lhs != rhs);
}

bool operator==(Timestamp lhs, Timestamp rhs)
{
    return lhs.raw() == rhs.raw();
}

void serialize(const Timestamp& ts, OutputArchive& ar)
{
     serialize(host_cast(ts.raw()), ar);
}

void deserialize(Timestamp& ts, InputArchive& ar)
{
    Timestamp::value_type tmp;
    deserialize(tmp, ar);
    ts = Timestamp { Timestamp::time_type::from_value(tmp) };
}

} // namespace geonet
} // namespace vanetza

