#ifndef CERTIFY_KEYIO_HPP
#define CERTIFY_KEYIO_HPP

#include <cryptopp/cryptlib.h>
#include <string>
#include <vanetza/security/backend_cryptopp.hpp>
#include <vanetza/security/ecdsa256.hpp>

void save_key(const std::string&, const CryptoPP::BufferedTransformation&);

void load_key(const std::string&, CryptoPP::BufferedTransformation&);

vanetza::security::ecdsa256::KeyPair to_keypair(const vanetza::security::BackendCryptoPP::PrivateKey&);

#endif /* CERTIFY_KEYIO_HPP */
