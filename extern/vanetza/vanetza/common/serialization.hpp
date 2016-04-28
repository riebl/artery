#ifndef SERIALIZATION_HPP_U2YGHSPB
#define SERIALIZATION_HPP_U2YGHSPB

#include <vanetza/common/byte_order.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/units/quantity.hpp>
#include <type_traits>

namespace vanetza
{

typedef boost::archive::binary_iarchive InputArchive;
typedef boost::archive::binary_oarchive OutputArchive;

template<typename T, ByteOrder ORDER>
void serialize(OutputArchive& ar, EndianType<T, ORDER> value)
{
    typedef typename decltype(value)::network_type network_type;
    T tmp = static_cast<network_type>(value).get();
    ar << tmp;
}

template<typename T, ByteOrder ORDER>
void deserialize(InputArchive& ar, EndianType<T, ORDER>& value)
{
    T tmp;
    ar >> tmp;
    value = network_cast<T>(tmp);
}

template<typename T>
typename std::enable_if<std::is_integral<T>::value>::type
deserialize(InputArchive& ar, T& value)
{
    T tmp;
    ar >> tmp;
    value = ntoh(tmp);
}

template<typename U, typename T>
void serialize(OutputArchive& ar, boost::units::quantity<U, T> q)
{
    static_assert(std::is_integral<T>::value,
            "Only integral based quantities are supported");
    auto tmp = hton(q.value());
    ar << tmp;
}

template<typename U, typename T>
void deserialize(InputArchive& ar, boost::units::quantity<U, T>& q)
{
    static_assert(std::is_integral<T>::value,
            "Only integral based quantities are supported");
    T tmp;
    ar >> tmp;
    q = boost::units::quantity<U, T>::from_value(ntoh(tmp));
}

template<class T>
typename std::enable_if<std::is_enum<T>::value>::type
serialize(OutputArchive& ar, const T& t)
{
    serialize(ar, host_cast(static_cast<typename std::underlying_type<T>::type const>(t)));
}

template<class T>
typename std::enable_if<std::is_enum<T>::value>::type
deserialize(InputArchive& ar, T& t)
{
    typename std::underlying_type<T>::type tmp;
    deserialize(ar, tmp);
    t = static_cast<T>(tmp);
}

} // namespace vanetza

#endif /* SERIALIZATION_HPP_U2YGHSPB */

