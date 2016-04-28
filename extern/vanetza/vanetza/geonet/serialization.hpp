#ifndef SERIALIZATION_HPP_XBJELAF0
#define SERIALIZATION_HPP_XBJELAF0

#include <vanetza/common/serialization.hpp>

namespace vanetza
{
namespace geonet
{

using vanetza::InputArchive;
using vanetza::OutputArchive;

template<typename T>
void serialize(T&& t, OutputArchive& oa)
{
    using vanetza::serialize;
    serialize(oa, std::forward<T>(t));
}

template<typename T>
void deserialize(T&& t, InputArchive& ia)
{
    using vanetza::deserialize;
    deserialize(ia, std::forward<T>(t));
}

} // namespace geonet
} // namespace vanetza

#endif /* SERIALIZATION_HPP_XBJELAF0 */

