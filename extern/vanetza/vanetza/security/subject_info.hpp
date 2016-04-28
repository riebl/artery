#ifndef SUBJECT_INFO_HPP_WCKSWSKY
#define SUBJECT_INFO_HPP_WCKSWSKY

#include <vanetza/security/serialization.hpp>
#include <vanetza/common/byte_buffer.hpp>
#include <array>
#include <cstdint>

namespace vanetza
{
namespace security
{

/// described in TS 103 097 v1.2.1, section 6.3
enum class SubjectType : uint8_t
{
    Enrollment_Credential = 0,
    Authorization_Ticket = 1,
    Authorization_Authority = 2,
    Enrollment_Atuhority = 3,
    Root_Ca = 4,
    Crl_Signer = 5
};

/// described in TS 103 097 v1.2.1, section 6.2
struct SubjectInfo
{
    SubjectType subject_type;
    ByteBuffer subject_name;
};

/**
 * \brief Serializes a SubjectInfo into a binary archive
 */
void serialize(OutputArchive&, const SubjectInfo&);

/**
 * \brief Deserializes a SubjectInfo from a binary archive
 * \param archive with a serialized SubjectInfo at the beginning
 * \param SubjectInfo
 * \return size of the deserialized SubjectInfo
 */
size_t deserialize(InputArchive&, SubjectInfo&);

/**
 * \brief Calculates size of a SubjectInfo
 * \param SubjectInfo
 * \return number of octets needed to serialize SubjectInfo
 */
size_t get_size(const SubjectInfo&);

} // namespace security
} // namespace vanetza

#endif /* SUBJECT_INFO_HPP_WCKSWSKY */
