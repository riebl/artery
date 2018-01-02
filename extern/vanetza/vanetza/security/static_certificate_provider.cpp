#include <vanetza/security/static_certificate_provider.hpp>

namespace vanetza
{
namespace security
{

StaticCertificateProvider::StaticCertificateProvider(const Certificate& authorization_ticket, const ecdsa256::KeyPair& authorization_ticket_key) :
    authorization_ticket(authorization_ticket), authorization_ticket_key(authorization_ticket_key) { }

const ecdsa256::PrivateKey& StaticCertificateProvider::own_private_key()
{
    return authorization_ticket_key.private_key;
}

const Certificate& StaticCertificateProvider::own_certificate()
{
    return authorization_ticket;
}

} // namespace security
} // namespace vanetza
