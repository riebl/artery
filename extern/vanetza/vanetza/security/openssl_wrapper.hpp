#ifndef OPENSSL_WRAPPER_HPP_GUFJGICT
#define OPENSSL_WRAPPER_HPP_GUFJGICT

#include <vanetza/common/byte_buffer.hpp>
#include <boost/core/noncopyable.hpp>
#include <openssl/bn.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <array>
#include <cstdint>
#include <stdexcept>

namespace vanetza
{
namespace security
{

// forward declaration
struct EcdsaSignature;

namespace openssl
{

/**
 * Check valid condition
 * \param valid If false stored OpenSSL error will be thrown
 */
void check(bool valid);


class Exception : public std::runtime_error
{
public:
    using code_type = decltype(ERR_get_error());

    Exception();
    explicit Exception(code_type err);
};


class BigNumber : private boost::noncopyable
{
public:
    BigNumber();
    ~BigNumber();

    BigNumber(const ByteBuffer& buf) :
        BigNumber(buf.data(), buf.size())
    {
    }

    template<std::size_t N>
    BigNumber(const std::array<uint8_t, N>& bin) :
        BigNumber(bin.data(), bin.size())
    {
    }

    operator BIGNUM*() { return bignum; }
    BIGNUM* move();

private:
    BigNumber(const uint8_t*, std::size_t);

    BIGNUM* bignum;
};


class BigNumberContext : private boost::noncopyable
{
public:
    BigNumberContext();
    ~BigNumberContext();

    operator BN_CTX*() { return ctx; }

private:
    BN_CTX* ctx;
};


class Point : private boost::noncopyable
{
public:
    explicit Point(const EC_GROUP* group);
    ~Point();

    operator EC_POINT*() { return point; }

private:
    EC_POINT* point;
};


class Signature : private boost::noncopyable
{
public:
    explicit Signature(ECDSA_SIG* sig);
    explicit Signature(const EcdsaSignature& sig);
    ~Signature();

    operator const ECDSA_SIG*() { return signature; }
    const ECDSA_SIG* operator->() const { return signature; }
    ECDSA_SIG* operator->() { return signature; }
    operator bool() { return signature != nullptr; }

private:
    ECDSA_SIG* signature;
};


class Key
{
public:
    Key();
    explicit Key(int nid);
    // non-copyable
    Key(const Key&) = delete;
    Key& operator=(const Key&) = delete;
    // but movable
    Key(Key&&);
    Key& operator=(Key&&);
    ~Key();

    operator EC_KEY*() { return eckey; }

private:
    EC_KEY* eckey;
};

} // namespace openssl
} // namespace security
} // namespace vanetza

#endif /* OPENSSL_WRAPPER_HPP_GUFJGICT */

