#ifndef BIT_NUMBER_HPP_H3ODBQR7
#define BIT_NUMBER_HPP_H3ODBQR7

#include <boost/operators.hpp>
#include <cstddef>
#include <type_traits>

namespace vanetza {

template<typename T, std::size_t WIDTH>
class BitNumber : public boost::totally_ordered<BitNumber<T, WIDTH>>
{
    static_assert(std::is_integral<T>::value == true,
            "only integral types are supported");
    static_assert(sizeof(T) * 8 > WIDTH,
            "width has to be less than size of underlying type");

public:
    static const T mask = (1 << WIDTH) - 1;
    static const std::size_t bits = WIDTH;
    typedef T value_type;

    BitNumber(): mValue(0) {}
    BitNumber(T value) : mValue(value & mask) {}
    BitNumber& operator=(T value) { mValue = value & mask; return *this; }
    T raw() const { return mValue; }

    bool operator<(BitNumber other) const { return mValue < other.mValue; }
    bool operator==(BitNumber other) const { return mValue == other.mValue; }

private:
    T mValue;
};

} // namespace vanetza

#endif /* BIT_NUMBER_HPP_H3ODBQR7 */

