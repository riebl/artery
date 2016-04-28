#ifndef CHECK_SUBJECT_ATTRIBUTE_HPP_40Z9HDV2
#define CHECK_SUBJECT_ATTRIBUTE_HPP_40Z9HDV2

#include <vanetza/security/subject_attribute.hpp>
#include <vanetza/security/tests/check_basic_elements.hpp>
#include <vanetza/security/tests/check_ecc_point.hpp>
#include <vanetza/security/tests/check_list.hpp>

namespace vanetza
{
namespace security
{

void check(const VerificationKey&, const VerificationKey&);
void check(const EncryptionKey&, const EncryptionKey&);
void check(const SubjectAssurance&, const SubjectAssurance&);
void check(const ItsAidSsp&, const ItsAidSsp&);
void check(const SubjectAttribute&, const SubjectAttribute&);

VerificationKey create_random_verification_key(int seed = 0);
EncryptionKey create_random_encryption_key(int seed = 0);
IntX create_random_its_aid(int seed = 0);
ItsAidSsp create_random_its_aid_ssp(int seed = 0);

} // namespace security
} // namespace vanetza

#endif /* CHECK_SUBJECT_ATTRIBUTE_HPP_40Z9HDV2 */

