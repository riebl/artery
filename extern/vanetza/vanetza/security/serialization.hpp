#ifndef SERIALIZATION_HPP_IENSIAL4
#define SERIALIZATION_HPP_IENSIAL4

#include <vanetza/common/serialization.hpp>
#include <vanetza/security/length_coding.hpp>
#include <cassert>
#include <list>

namespace vanetza
{
namespace security
{

using vanetza::serialize;
using vanetza::deserialize;

/**
 * \brief Serialize given length
 * \param ar to serialize in
 * \param size to encode
 */
void serialize_length(OutputArchive&, size_t);

/**
 * \brief Deserialize length from a given archive
 * \param ar shall start with encoded length
 * \return length deserialized from archive
 */
size_t deserialize_length(InputArchive&);

/**
 * \brief Calculate size of a list
 *
 * Sums up sizes of all list elements only, length itself is not included.
 * Therefore, the returned length is suitable as argument for serialize_length.
 *
 * \tparam T list element type
 * \param list
 * \return accumulated elements' size
 */
template<class T>
size_t get_size(const std::list<T>& list)
{
    size_t size = 0;
    for (auto& elem : list) {
        size += get_size(elem);
    }
    return size;
}

/** \brief Serialize from any given list into given binary archive
 * \tparam T the type of the list
 * \tparam ARGS all additional arguments for the underlying functions
 * \param ar to serialize in
 * \param list
 * \param args the additional arguments
 */
template<class T, typename... ARGS>
void serialize(OutputArchive& ar, const std::list<T>& list, ARGS&&... args)
{
    size_t size = get_size(list);
    serialize_length(ar, size);
    for (auto& elem : list) {
        serialize(ar, elem, std::forward<ARGS>(args)...);
    }
}

/** \brief Deserialize a list from given archive
 * \tparam T the type of the list
 * \tparam ARGS all additional arguments for the underlying functions
 * \param ar, shall start with the list
 * \param args the additional arguments
 * \return size of the deserialized list in bytes
 */
template<class T, typename... ARGS>
size_t deserialize(InputArchive& ar, std::list<T>& list, ARGS&&... args)
{
    const size_t length = deserialize_length(ar);
    int remainder = length;
    while (remainder > 0) {
        T t;
        remainder -= deserialize(ar, t, std::forward<ARGS>(args)...);
        list.push_back(std::move(t));
    }
    assert(remainder == 0);
    return length;
}

} // namespace security
} // namespace vanetza

#endif /* SERIALIZATION_HPP_IENSIAL4 */
