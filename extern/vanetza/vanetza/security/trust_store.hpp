#ifndef VANETZA_TRUST_STORE_HPP
#define VANETZA_TRUST_STORE_HPP

#include <vanetza/security/certificate.hpp>
#include <map>
#include <vector>

namespace vanetza
{
namespace security
{

class TrustStore
{
public:
    /**
     * Create trust store with given trusted certificates
     * \param trusted_certificates these certificates are copied into trust store
     */
    TrustStore(const std::vector<Certificate>& trusted_certificates);

    /**
     * Look up certificates by hash id
     * \param id hash identifier of certificate
     * \return all stored certificates matching hash id
     */
    std::vector<Certificate> find_by_id(HashedId8 id) const;

private:
    std::multimap<HashedId8, Certificate> certificates;
};

} // namespace security
} // namespace vanetza

#endif /* VANETZA_TRUST_STORE_HPP */
