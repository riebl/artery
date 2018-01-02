#include <vanetza/security/trust_store.hpp>

namespace vanetza
{
namespace security
{

TrustStore::TrustStore(const std::vector<Certificate>& trusted_certificates)
{
    for (const auto& certificate : trusted_certificates) {
        HashedId8 id = calculate_hash(certificate);
        certificates.insert(std::make_pair(id, certificate));
    }
}

std::vector<Certificate> TrustStore::find_by_id(HashedId8 id) const
{
    using iterator = std::multimap<HashedId8, Certificate>::const_iterator;
    std::pair<iterator, iterator> range = certificates.equal_range(id);

    std::vector<Certificate> matching_certificates;

    for (auto item = range.first; item != range.second; ++item) {
        matching_certificates.push_back(item->second);
    }

    return matching_certificates;
}

} // namespace security
} // namespace vanetza
