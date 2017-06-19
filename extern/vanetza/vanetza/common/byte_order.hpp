#ifndef BYTE_ORDER_HPP_LPUJ094I
#define BYTE_ORDER_HPP_LPUJ094I

#include <cstdint>
#include <functional>
#include <iosfwd>
#include <type_traits>
#include <boost/endian/conversion.hpp>

namespace vanetza
{

template<class T>
T hton(T host_value)
{
    return boost::endian::native_to_big(host_value);
}

template<class T>
T ntoh(T network_value)
{
    return boost::endian::big_to_native(network_value);
}


enum class ByteOrder {
    BigEndian,
    LittleEndian
};


namespace detail
{

#if BYTE_ORDER == LITTLE_ENDIAN
    static constexpr ByteOrder host_byte_order = ByteOrder::LittleEndian;
#elif BYTE_ORDER == BIG_ENDIAN
    static constexpr ByteOrder host_byte_order = ByteOrder::BigEndian;
#else
#   error "Unknown byte order"
#endif

} // namespace detail

constexpr ByteOrder getHostByteOrder() { return detail::host_byte_order; }

template<typename T, ByteOrder ORDER = getHostByteOrder()>
class EndianType;

/**
 * Explicitly forge a plain type to an EndianType.
 * It is assumed the passed value is already in the stated byte order,
 * i.e. endian_cast does _not_ trigger any automatic conversions.
 * \param value A plain value in byte order ORDER
 * \return EndianType capable to carry value type
 */
template<ByteOrder ORDER, typename T>
EndianType<T, ORDER> endian_cast(T value)
{
    return static_cast< EndianType<T, ORDER> >(value);
}

/**
 * Cast POD type to EndianType in host byte order
 * \param value POD in host byte order
 * \return EndianType carrying value
 */
template<typename T>
EndianType<T, getHostByteOrder()> host_cast(T value)
{
    return endian_cast<getHostByteOrder()>(value);
}

/**
 * Cast POD type to EndianType in network byte order
 * \param value POD in network byte order
 * \return EndianType carrying value
 */
template<typename T>
EndianType<T, ByteOrder::BigEndian> network_cast(T value)
{
    return endian_cast<ByteOrder::BigEndian>(value);
}


namespace detail
{

template<typename T, ByteOrder FROM, ByteOrder TO>
struct EndianConverter
{
    T operator()(const T&) const;
};

template<typename T, ByteOrder ORDER>
struct EndianConverter<T, ORDER, ORDER>
{
    T operator()(const T& t) const { return t; }
};

template<typename T>
struct EndianConverter<T, ByteOrder::LittleEndian, ByteOrder::BigEndian>
{
    T operator()(const T& t) const { return boost::endian::endian_reverse(t); }
};

template<typename T>
struct EndianConverter<T, ByteOrder::BigEndian, ByteOrder::LittleEndian>
{
    T operator()(const T& t) const { return boost::endian::endian_reverse(t); }
};

template<typename T, ByteOrder FROM, ByteOrder TO>
T convert_endian(const T& t)
{
    EndianConverter<T, FROM, TO> converter;
    return converter(t);
}

} // namespace detail


template<typename T, ByteOrder ORDER>
class EndianType
{
public:
    static_assert(std::is_pod<T>::value == true, "EndianType is only availabe for POD types");
    typedef T value_type;
    typedef EndianType<T, getHostByteOrder()> host_type;
    typedef EndianType<T, ByteOrder::BigEndian> network_type;

    EndianType() = default;
    explicit EndianType(T value) { m_value = value; }

    EndianType(const EndianType&) = default;
    EndianType& operator=(const EndianType&) = default;

    template<ByteOrder OTHER_ORDER>
    EndianType(const EndianType<T, OTHER_ORDER>& other) :
        m_value(detail::convert_endian<T, OTHER_ORDER, ORDER>(other.m_value))
    {
    }

    template<ByteOrder OTHER_ORDER>
    EndianType& operator=(const EndianType<T, OTHER_ORDER>& other)
    {
        m_value = detail::convert_endian<T, OTHER_ORDER, ORDER>(other.m_value);
        return *this;
    }

    bool operator==(const EndianType& other) const
    {
        return m_value == other.m_value;
    }

    bool operator!=(const EndianType& other) const
    {
        return !(*this == other);
    }

    bool operator<(const EndianType& other) const
    {
        return m_value < other.m_value;
    }

    value_type net() const
    {
        return detail::convert_endian<T, ORDER, ByteOrder::BigEndian>(m_value);
    }

    value_type host() const
    {
        return detail::convert_endian<T, ORDER, getHostByteOrder()>(m_value);
    }

    value_type get() const
    {
        return m_value;
    }

private:
    friend class EndianType<T, ByteOrder::BigEndian>;
    friend class EndianType<T, ByteOrder::LittleEndian>;

    T m_value;
};


/**
 * Print to ostream in network byte order
 * \param os output stream
 * \param t endian type object
 * \return os
 */
template<typename T, ByteOrder ORDER>
std::ostream& operator<<(std::ostream& os, const EndianType<T, ORDER>& t)
{
    os << t.net();
    return os;
}

typedef EndianType<uint8_t, ByteOrder::BigEndian> uint8be_t;
typedef EndianType<uint16_t, ByteOrder::BigEndian> uint16be_t;
typedef EndianType<uint32_t, ByteOrder::BigEndian> uint32be_t;
typedef EndianType<uint64_t, ByteOrder::BigEndian> uint64be_t;

typedef EndianType<int8_t, ByteOrder::BigEndian> int8be_t;
typedef EndianType<int16_t, ByteOrder::BigEndian> int16be_t;
typedef EndianType<int32_t, ByteOrder::BigEndian> int32be_t;
typedef EndianType<int64_t, ByteOrder::BigEndian> int64be_t;

} // namespace vanetza

namespace std
{

template<typename T, vanetza::ByteOrder ORDER>
struct hash<vanetza::EndianType<T, ORDER>>
{
    size_t operator()(const vanetza::EndianType<T, ORDER>& t) const
    {
        return hash<T>()(t.get());
    }
};

} // namespace std

#endif /* BYTE_ORDER_HPP_LPUJ094I */

