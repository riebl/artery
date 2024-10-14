/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_CP_UTILITY_H_
#define ARTERY_CP_UTILITY_H_

/**
 * @file Utility.h
 * @brief Various helper functions for CPM's
 */

#include "artery/application/VehicleDataProvider.h"

#include <boost/ptr_container/ptr_vector.hpp>
#include <vanetza/asn1/asn1c_wrapper.hpp>
#include <vanetza/asn1/its/ObjectClass.h>
#include <vanetza/asn1/its/PerceivedObject.h>
#include <vanetza/asn1/its/SensorInformationContainer.h>
#include <vanetza/asn1/support/asn_SEQUENCE_OF.h>
#include <vanetza/asn1/support/constr_TYPE.h>

#include <cassert>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>


/**
 * @brief New method required by the Clone Allocator concept of boost::ptr_vector
 *
 * Must be defined in global scope for ADL to work.
 *
 * @param t Object to clone
 * @return Clone of t
 */
PerceivedObject* new_clone(const PerceivedObject& t);
/**
 * @brief Delete method required by the Clone Allocator concept of boost::ptr_vector
 *
 * Must be defined in global scope for ADL to work.
 *
 * @param t Object to delete
 */
void delete_clone(const PerceivedObject* t);


namespace artery
{
namespace cp
{

/**
 * @brief Deleter of owned ASN1 objects
 *
 * Deletes ASN1 objects that must have been created by vanetza::asn1::allocate().
 */
struct asn1_delete {
    /**
     * @brief ASN1 type descriptor of the object to delete, MUST match the object type to delete
     */
    asn_TYPE_descriptor_t& td;

    constexpr asn1_delete(asn_TYPE_descriptor_t& td) noexcept : td(td) {}
    void operator()(void* p) { vanetza::asn1::free(td, p); }
};

/**
 * @brief Create an owned ASN1 object
 *
 * Uses vanetza::asn1::allocate() to create the object.
 *
 * @tparam T ASN1 object class
 * @param td ASN1 type descriptor, must match the ASN1 object class
 * @return ASN1 object as owned pointer
 */
template <typename T>
std::unique_ptr<T, asn1_delete> make_asn1(asn_TYPE_descriptor_t& td)
{
    return std::unique_ptr<T, asn1_delete>(vanetza::asn1::allocate<T>(), asn1_delete(td));
}

/**
 * @brief Create an empty owned ASN1 object
 *
 * Helper function to create an owned ASN1 object nullptr because these can't be default constructed.
 *
 * @tparam T ASN1 object class
 * @param td ASN1 type descriptor, must match the ASN1 object class
 * @return nullptr of the matching type
 */
template <typename T>
std::unique_ptr<T, asn1_delete> make_empty_asn1(asn_TYPE_descriptor_t& td)
{
    return std::unique_ptr<T, asn1_delete>(static_cast<T*>(nullptr), asn1_delete(td));
}

/**
 * @brief Sensor Information Container owning pointer
 */
using SensorInformationContainer_ptr = std::unique_ptr<SensorInformationContainer, asn1_delete>;


/**
 * @brief Vector of owned Perceived Objects
 *
 * This vector is more lightweight than a PerceivedObjectContainer
 * and allows some additional operations to be easily performed like sorting.
 *
 * Objects added to this container must have been created by vanetza::asn1::allocate().
 */
using PerceivedObjects = boost::ptr_vector<PerceivedObject>;


/**
 * @brief Set an owned ASN1 object to a field
 *
 * The ownership gets transferred.
 *
 * @tparam T ASN1 class
 * @param field ASN1 field
 * @param ptr Pointer of ASN1 object
 */
template <typename T>
void asn1_field_set(T*& field, std::unique_ptr<T, asn1_delete>& ptr)
{
    field = ptr.release();
}

/**
 * @brief Get an owned ASN1 object from a field
 *
 * The ownership gets transferred.
 *
 * @tparam T ASN1 class
 * @param field ASN1 field
 * @param ptr Pointer of ASN1 object
 */
template <typename T>
void asn1_field_get(T*& field, std::unique_ptr<T, asn1_delete>& ptr)
{
    ptr.reset(field);
    field = nullptr;
}

/**
 * @brief Add an owned ASN1 object to a sequence
 *
 * The ownership gets transferred.
 * The sequence MUST store the same type of ASN1 objects like the pointer.
 *
 * @tparam T Pointer type
 * @param asn_sequence_of_x Sequence of ASN1 objects
 * @param ptr Pointer of ASN1 object, must not be nullptr
 * @throw std::runtime_error Memory allocation error
 */
template <typename T>
typename std::enable_if<std::is_same<std::unique_ptr<typename T::element_type, asn1_delete>, T>::value>::type asn1_sequence_add(void* asn_sequence_of_x, T& ptr)
{
    assert(asn_sequence_of_x && ptr.operator->());
    if (ASN_SEQUENCE_ADD(asn_sequence_of_x, ptr.operator->())) {
        throw std::runtime_error("Bad ASN.1 memory allocation");
    }
    ptr.release();
}

/**
 * @brief Push an owned object from a vector into a sequence
 *
 * The last element of the vector gets removed and inserted at the end of the sequence, the ownership gets transferred.
 * The sequence MUST store the same type of ASN1 objects like the vector.
 *
 * @tparam T Vector type
 * @param asn_sequence_of_x Sequence of ASN1 objects
 * @param vec Vector of ASN1 objects, maybe be empty
 * @return 0 if successful, 1 if the vector was empty
 * @throw std::runtime_error Memory allocation error
 */
template <typename T>
typename std::enable_if<std::is_same<boost::ptr_vector<typename std::remove_pointer<typename T::value_type>::type>, T>::value, int>::type asn1_sequence_push(
    void* asn_sequence_of_x, T& vec)
{
    assert(asn_sequence_of_x);
    if (vec.empty()) {
        return 1;
    }

    auto item = vec.pop_back();
    if (ASN_SEQUENCE_ADD(asn_sequence_of_x, item.operator->())) {
        throw std::runtime_error("Bad ASN.1 memory allocation");
    }
    item.release();

    return 0;
}

/**
 * @brief Pop an owned object from a sequence into a vector
 *
 * The last element of the sequence gets removed and inserted at the end of the vector, the ownership gets transferred.
 * The sequence MUST store the same type of ASN1 objects like the vector.
 *
 * Implementation note: Relies on internal knowledge about A_SEQUENCE_OF() sequences.
 *
 * @tparam T Vector type
 * @param asn_sequence_of_x Sequence of ASN1 objects, may be empty
 * @param vec Vector of ASN1 objects
 * @return 0 if successful, 1 if the sequence was empty
 * @throw std::runtime_error Memory allocation error
 */
template <typename T>
typename std::enable_if<std::is_same<boost::ptr_vector<typename std::remove_pointer<typename T::value_type>::type>, T>::value, int>::type asn1_sequence_pop(
    void* asn_sequence_of_x, T& vec)
{
    // It seems there is no "public" API available to remove items from a sequence,
    // all small letter macros / functions look like they are "private".
    // Since there is not even a "private" function to query the number of items,
    // use internal knowledge to get that count and also to remove the last item.
    assert(asn_sequence_of_x);
    asn_anonymous_sequence_* as = _A_SEQUENCE_FROM_VOID(asn_sequence_of_x);
    if (as->count <= 0) {
        return 1;
    }

    vec.push_back(static_cast<typename T::value_type>(as->array[--as->count]));

    return 0;
}


/**
 * @brief Get the Object Class of an Artery station
 *
 * Maps an Artery station type to a pair of a CPM PerceivedObject ObjectClass and its Subclass.
 *
 * Implementation note: This uses the fact that all different subclass types are in fact a typedef of long,
 *                      this saves template magic to return the proper type.
 *
 * @param stationType Artery station
 * @return Object Class and Subclass
 */
std::pair<ObjectClass__class_PR, long> getObjectClass(artery::VehicleDataProvider::StationType stationType);

}  // namespace cp
}  // namespace artery

#endif /* ARTERY_CP_UTILITY_H_ */
