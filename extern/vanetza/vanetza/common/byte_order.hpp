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
    static const ByteOrder host_byte_order = ByteOrder::LittleEndian;
#elif BYTE_ORDER == BIG_ENDIAN
    static const ByteOrder host_byte_order = ByteOrder::BigEndian;
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

template<typename T>
class EndianTypeStorage
{
    static_assert(std::is_pod<T>::value == true, "EndianType is only availabe for POD types");

public:
    typedef T value_type;

    EndianTypeStorage() = default;
    explicit EndianTypeStorage(T value) : mValue(value) {}

    void set(T value) { mValue = value; }
    T get() const { return mValue; }
    explicit operator T() const { return get(); }

protected:
    T mValue;
};


template<typename T, typename BASE>
class EndianTypeCasterToHost : public BASE
{
public:
    typedef EndianType<T, getHostByteOrder()> host_type;
    operator host_type() const { return host_cast(ntoh(BASE::get())); }
};

template<typename T, typename BASE>
class EndianTypeCasterToNetwork : public BASE
{
public:
    typedef EndianType<T, ByteOrder::BigEndian> network_type;
    operator network_type() const { return network_cast(hton(BASE::get())); }
};


template<typename T, typename BASE, ByteOrder ORDER, bool HOST_EQ_NET = (getHostByteOrder() == ByteOrder::BigEndian)>
class EndianTypeCaster;

template<typename T, typename BASE, ByteOrder ORDER>
class EndianTypeCaster<T, BASE, ORDER, true> : public BASE {};

template<typename T, typename BASE, ByteOrder ORDER>
class EndianTypeCaster<T, BASE, ORDER, false> : public
    std::conditional<ORDER == ByteOrder::BigEndian,
        EndianTypeCasterToHost<T, BASE>,
        EndianTypeCasterToNetwork<T, BASE>
    >::type {};

} // namespace detail


template<typename T, ByteOrder ORDER>
class EndianType : public detail::EndianTypeCaster<T, detail::EndianTypeStorage<T>, ORDER>
{
public:
    typedef detail::EndianTypeStorage<T> storage_type;
    typedef detail::EndianTypeCaster<T, storage_type, ORDER> base_type;
    typedef EndianType<T, getHostByteOrder()> host_type;
    typedef EndianType<T, ByteOrder::BigEndian> network_type;

    EndianType() = default;
    explicit EndianType(T value) { storage_type::set(value); }

    bool operator==(const EndianType& other) const
    {
        return base_type::get() == other.get();
    }

    bool operator!=(const EndianType& other) const
    {
        return !(*this == other);
    }

    T net() const
    {
        return network_type(*this).get();
    }

    T host() const
    {
        return host_type(*this).get();
    }
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

