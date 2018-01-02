#include "auth-ca.hpp"
#include "keyio.hpp"
#include <boost/program_options.hpp>
#include <chrono>
#include <cryptopp/eccrypto.h>
#include <cryptopp/oids.h>
#include <cryptopp/osrng.h>
#include <cryptopp/queue.h>
#include <cryptopp/sha.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vanetza/common/clock.hpp>
#include <vanetza/security/backend_cryptopp.hpp>
#include <vanetza/security/basic_elements.hpp>
#include <vanetza/security/certificate.hpp>
#include <vanetza/security/subject_attribute.hpp>
#include <vanetza/security/subject_info.hpp>

namespace po = boost::program_options;
namespace vs = vanetza::security;
using namespace CryptoPP;

void AuthCaCommand::parse(const std::vector<std::string>& opts)
{
    po::options_description desc("auth-ca options");
    desc.add_options()
        ("output", po::value<std::string>(&output)->required(), "Output file.")
        ("sign-key", po::value<std::string>(&sign_key)->required(), "Private key file of the signer.")
        ("sign-cert", po::value<std::string>(&sign_cert)->required(), "Private certificate file of the signer.")
        ("subject-key", po::value<std::string>(&subject_key)->required(), "Private key file to issue the certificate for.")
        ("subject-name", po::value<std::string>(&subject_name)->default_value("Hello World Auth-CA"), "Subject name.")
        ("days", po::value<int>(&validity_days)->default_value(180), "Validity in days.")
    ;

    po::positional_options_description pos;
    pos.add("output", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(opts).options(desc).positional(pos).run(), vm);
    po::notify(vm);
}

int AuthCaCommand::execute()
{
    std::cout << "Loading keys... ";

    AutoSeededRandomPool rng;
    vs::BackendCryptoPP crypto_backend;

    ECDSA<ECP, SHA256>::PrivateKey loaded_sign_key;
    ECDSA<ECP, SHA256>::PrivateKey loaded_subject_key;

    ByteQueue queue;
    load_key(sign_key, queue);
    loaded_sign_key.Load(queue);

    if (!loaded_sign_key.Validate(rng, 3)) {
        throw std::runtime_error("Private key validation failed for sign key");
    }

    queue.Clear();
    load_key(subject_key, queue);
    loaded_subject_key.Load(queue);

    if (!loaded_subject_key.Validate(rng, 3)) {
        throw std::runtime_error("Private key validation failed for subject key");
    }

    std::cout << "OK" << std::endl;

    vs::Certificate loaded_sign_cert;

    std::ifstream src;
    src.open(sign_cert.c_str(), std::ios::in | std::ios::binary);

    vanetza::InputArchive iarchive(src);
    vs::deserialize(iarchive, loaded_sign_cert);

    auto sign_key_pair = to_keypair(loaded_sign_key);
    auto subject_key_pair = to_keypair(loaded_subject_key);
    auto time_now = vanetza::Clock::at(boost::posix_time::microsec_clock::universal_time());

    vs::Certificate certificate;

    certificate.signer_info = vs::calculate_hash(loaded_sign_cert);

    std::vector<unsigned char> subject(subject_name.begin(), subject_name.end());
    certificate.subject_info.subject_name = subject;
    certificate.subject_info.subject_type = vs::SubjectType::Authorization_Authority;

    certificate.subject_attributes.push_back(vs::SubjectAssurance(0x00));

    vs::Uncompressed coordinates;
    coordinates.x.assign(subject_key_pair.public_key.x.begin(), subject_key_pair.public_key.x.end());
    coordinates.y.assign(subject_key_pair.public_key.y.begin(), subject_key_pair.public_key.y.end());
    vs::EccPoint ecc_point = coordinates;
    vs::ecdsa_nistp256_with_sha256 ecdsa;
    ecdsa.public_key = ecc_point;
    vs::VerificationKey verification_key;
    verification_key.key = ecdsa;
    certificate.subject_attributes.push_back(verification_key);

    vs::StartAndEndValidity start_and_end;
    start_and_end.start_validity = vs::convert_time32(time_now - std::chrono::hours(1));
    start_and_end.end_validity = vs::convert_time32(time_now + std::chrono::hours(24 * validity_days));
    certificate.validity_restriction.push_back(start_and_end);

    std::cout << "Signing certificate... ";

    vanetza::ByteBuffer data_buffer = vs::convert_for_signing(certificate);
    certificate.signature = crypto_backend.sign_data(sign_key_pair.private_key, data_buffer);

    std::cout << "OK" << std::endl;

    std::cout << "Writing certificate to '" << output << "'... ";

    std::ofstream dest;
    dest.open(output.c_str(), std::ios::out | std::ios::binary);

    vanetza::OutputArchive archive(dest);
    vs::serialize(archive, certificate);

    std::cout << "OK" << std::endl;

    return 0;
}
