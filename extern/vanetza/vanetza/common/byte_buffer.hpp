#ifndef BYTE_BUFFER_HPP_7NOEQO4F
#define BYTE_BUFFER_HPP_7NOEQO4F

#include <cstdint>
#include <type_traits>
#include <vector>

namespace vanetza
{

typedef std::vector<uint8_t> ByteBuffer;

/**
 * Cast byte buffer to a POD object
 * \param buffer byte buffer containing requested object
 * \return pointer to object or nullptr if cast is impossible
 */
template<typename MASK>
MASK* buffer_cast(ByteBuffer& buffer)
{
    static_assert(std::is_pod<MASK>::value, "MASK has to be POD data type");
    static_assert(std::is_object<MASK>::value, "MASK has to be an object");

    MASK* mask = nullptr;
    if (sizeof(MASK) <= buffer.size()) {
        mask = reinterpret_cast<MASK*>(&buffer[0]);
    }
    return mask;
}

template<typename MASK>
const MASK* buffer_cast(const ByteBuffer& buffer)
{
    // const_cast is safe, const qualifier is added to return type
    return buffer_cast<MASK>(const_cast<ByteBuffer&>(buffer));
}

/**
 * Create byte buffer with copy of POD object
 * \param obj POD object
 * \return ByteBuffer object with copy
 */
template<class T>
ByteBuffer buffer_copy(const T& object)
{
    static_assert(std::is_pod<T>::value, "T has to be POD data type");
    auto ptr = reinterpret_cast<const uint8_t*>(&object);
    return ByteBuffer(ptr, ptr + sizeof(T));
}

} // namespace vanetza

#endif /* BYTE_BUFFER_HPP_7NOEQO4F */

