#ifndef ASN1C_WRAPPER_HPP_ZCNDO8E5
#define ASN1C_WRAPPER_HPP_ZCNDO8E5

#include <vanetza/asn1/gen/asn_system.h>
#include <vanetza/asn1/gen/constr_TYPE.h>
#include <vanetza/common/byte_buffer.hpp>
#include <cstddef>
#include <string>
#include <utility>

namespace vanetza
{
namespace asn1
{

void* allocate(std::size_t);
void free(asn_TYPE_descriptor_t&, void*);
void* copy(asn_TYPE_descriptor_t&, const void*);
bool validate(asn_TYPE_descriptor_t&, const void*);
bool validate(asn_TYPE_descriptor_t&, const void*, std::string&);
std::size_t size(asn_TYPE_descriptor_t&, const void*);
ByteBuffer encode(asn_TYPE_descriptor_t&, const void*);
bool decode(asn_TYPE_descriptor_t&, void**, const ByteBuffer&);

template<class T>
T* allocate()
{
    return static_cast<T*>(allocate(sizeof(T)));
}

template<class T>
class asn1c_wrapper
{
public:
    typedef T asn1c_type;

    asn1c_wrapper(asn_TYPE_descriptor_t& desc) :
        m_struct(vanetza::asn1::allocate<asn1c_type>()), m_type(desc) {}
    ~asn1c_wrapper() { vanetza::asn1::free(m_type, m_struct); }

    // copy semantics
    asn1c_wrapper(const asn1c_wrapper& other) :
        m_struct(nullptr), m_type(other.m_type)
    {
        void* copy = vanetza::asn1::copy(m_type, other.m_struct);
        m_struct = static_cast<asn1c_type*>(copy);
    }

    asn1c_wrapper& operator=(const asn1c_wrapper& other)
    {
        using namespace vanetza::asn1;
        free(m_type, m_struct);
        void* dup = copy(other.m_type, other.m_struct);
        m_struct = static_cast<asn1c_type*>(dup);
        m_type = other.m_type;
        return *this;
    }

    // move semantics
    asn1c_wrapper(asn1c_wrapper&& other) :
        m_struct(nullptr), m_type(other.m_type)
    {
        std::swap(m_struct, other.m_struct);
    }

    asn1c_wrapper& operator=(asn1c_wrapper&& other)
    {
        std::swap(m_struct, other.m_struct);
    }

    // dereferencing
    asn1c_type& operator*() { return *m_struct; }
    asn1c_type* operator->() { return m_struct; }
    const asn1c_type& operator*() const { return *m_struct; }
    const asn1c_type* operator->() const { return m_struct; }

    /**
     * Check ASN.1 constraints
     * \param error (optional) copy of error message
     * \return true if valid
     */
    bool validate() const
    {
        return vanetza::asn1::validate(m_type, m_struct);
    }

    /**
     * Check ASN.1 constraints
     * \param error Error message if any constraint failed
     * \return true if valid
     */
    bool validate(std::string& error) const
    {
        return vanetza::asn1::validate(m_type, m_struct, error);
    }

    /**
     * Encode ASN.1 struct into byte buffer
     * \return byte buffer containing serialized ASN.1 struct
     */
    ByteBuffer encode() const
    {
        return vanetza::asn1::encode(m_type, m_struct);
    }

    /**
     * Try to decode ASN.1 struct from byte buffer
     * \param buffer input data
     * \return true if decoding has been successful
     */
    bool decode(const ByteBuffer& buffer)
    {
        return vanetza::asn1::decode(m_type, (void**)&m_struct, buffer);
    }

    /**
     * Get size of encoded ASN.1 struct
     * \return size in bytes
     */
    std::size_t size() const
    {
        return vanetza::asn1::size(m_type, m_struct);
    }

private:
    asn1c_type* m_struct;
    asn_TYPE_descriptor_t& m_type;
};

} // namespace asn1
} // namespace vanetza

#endif /* ASN1C_WRAPPER_HPP_ZCNDO8E5 */

