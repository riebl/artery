#include <vanetza/security/basic_elements.hpp>
#include <algorithm>
#include <cassert>
#include <chrono>

namespace vanetza
{
namespace security
{

HashedId3 truncate(const HashedId8& in)
{
    HashedId3 out;
    assert(out.size() <= in.size());
    std::copy_n(in.rbegin(), out.size(), out.rbegin());
    return out;
}

Time32 convert_time32(const Clock::time_point& tp)
{
    using namespace std::chrono;
    return duration_cast<seconds>(tp.time_since_epoch()).count();
}

Time64 convert_time64(const Clock::time_point& tp)
{
    using namespace std::chrono;
    return duration_cast<microseconds>(tp.time_since_epoch()).count();
}

} // namespace security
} // namespace vanetza
