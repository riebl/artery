#include <vanetza/security/openssl_wrapper.hpp>
#include <vanetza/security/public_key.hpp>
#include <vanetza/security/signature.hpp>
#include <cassert>

namespace vanetza
{
namespace security
{
namespace openssl
{

void check(bool valid)
{
    if (!valid) {
        throw Exception();
    }
}

Exception::Exception() : Exception(ERR_get_error())
{
}

Exception::Exception(code_type err) :
    std::runtime_error(ERR_reason_error_string(err))
{
}

BigNumber::BigNumber() : bignum(BN_new())
{
    check(bignum != nullptr);
}

BigNumber::BigNumber(const uint8_t* arr, std::size_t len) : BigNumber()
{
    BN_bin2bn(arr, len, bignum);
}

BIGNUM* BigNumber::move()
{
    BIGNUM* ptr = nullptr;
    std::swap(ptr, bignum);
    return ptr;
}

BigNumber::~BigNumber()
{
    if (bignum) {
        BN_clear_free(bignum);
    }
}

BigNumberContext::BigNumberContext() : ctx(BN_CTX_new())
{
    check(ctx != nullptr);
    BN_CTX_start(ctx);
}

BigNumberContext::~BigNumberContext()
{
    BN_CTX_end(ctx);
    BN_CTX_free(ctx);
}

Point::Point(const EC_GROUP* group) : point(EC_POINT_new(group))
{
    check(point != nullptr);
}

Point::~Point()
{
    EC_POINT_free(point);
}

Signature::Signature(ECDSA_SIG* sig) : signature(sig)
{
    check(signature);
}

Signature::Signature(const EcdsaSignature& ecdsa) : signature(ECDSA_SIG_new())
{
    check(signature);
#if OPENSSL_API_COMPAT < 0x10100000L
    const ByteBuffer r = convert_for_signing(ecdsa.R);
    BN_bin2bn(r.data(), r.size(), signature->r);
    BN_bin2bn(ecdsa.s.data(), ecdsa.s.size(), signature->s);
#else
    BigNumber bn_r { convert_for_signing(ecdsa.R) };
    BigNumber bn_s { ecdsa.s };
    // ownership of big numbers is transfered by calling ECDSA_SIG_set0!
    ECDSA_SIG_set0(signature, bn_r.move(), bn_s.move());
#endif
}

Signature::~Signature()
{
    ECDSA_SIG_free(signature);
}

Key::Key() : eckey(EC_KEY_new())
{
    check(eckey);
}

Key::Key(int nid) : eckey(EC_KEY_new_by_curve_name(nid))
{
    check(eckey);
}

Key::Key(Key&& other) : eckey(nullptr)
{
    std::swap(eckey, other.eckey);
}

Key& Key::operator=(Key&& other)
{
    std::swap(eckey, other.eckey);
    return *this;
}

Key::~Key()
{
    EC_KEY_free(eckey);
}

} // namespace openssl
} // namespace security
} // namespace vanetza
