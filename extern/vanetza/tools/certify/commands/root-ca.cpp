#include "keyio.hpp"
#include "root-ca.hpp"
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

void RootCaCommand::parse(const std::vector<std::string>& opts)
{
    po::options_description desc("root-ca options");
    desc.add_options()
        ("output", po::value<std::string>(&output)->required(), "Output file.")
        ("cert-key", po::value<std::string>(&cert_key)->required(), "Private key file.")
        ("subject-name", po::value<std::string>(&subject_name)->default_value("Hello World Root-CA"), "Subject name.")
        ("days", po::value<int>(&validity_days)->default_value(365), "Validity in days.")
    ;

    po::positional_options_description pos;
    pos.add("output", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(opts).options(desc).positional(pos).run(), vm);
    po::notify(vm);
}

int RootCaCommand::execute()
{
    std::cout << "Loading key... ";

    AutoSeededRandomPool rng;
    vs::BackendCryptoPP crypto_backend;
    ECDSA<ECP, SHA256>::PrivateKey private_key;

    ByteQueue queue;
    load_key(cert_key, queue);
    private_key.Load(queue);

    if (!private_key.Validate(rng, 3)) {
        throw std::runtime_error("Private key validation failed");
    }

    std::cout << "OK" << std::endl;

    auto key_pair = to_keypair(private_key);
    auto time_now = vanetza::Clock::at(boost::posix_time::microsec_clock::universal_time());

    // create certificate
    vs::Certificate certificate;

    // section 6.1 in TS 103 097 v1.2.1
    certificate.signer_info = nullptr; /* self */

    // section 6.3 in TS 103 097 v1.2.1
    certificate.subject_info.subject_type = vs::SubjectType::Root_Ca;

    // section 7.4.2 in TS 103 097 v1.2.1
    std::vector<unsigned char> subject(subject_name.begin(), subject_name.end());
    certificate.subject_info.subject_name = subject;

    // section 6.6 in TS 103 097 v1.2.1 - levels currently undefined
    certificate.subject_attributes.push_back(vs::SubjectAssurance(0x00));

    // section 7.4.1 in TS 103 097 v1.2.1
    // set subject attributes
    // set the verification_key
    vs::Uncompressed coordinates;
    coordinates.x.assign(key_pair.public_key.x.begin(), key_pair.public_key.x.end());
    coordinates.y.assign(key_pair.public_key.y.begin(), key_pair.public_key.y.end());
    vs::EccPoint ecc_point = coordinates;
    vs::ecdsa_nistp256_with_sha256 ecdsa;
    ecdsa.public_key = ecc_point;
    vs::VerificationKey verification_key;
    verification_key.key = ecdsa;
    certificate.subject_attributes.push_back(verification_key);

    // section 6.7 in TS 103 097 v1.2.1
    // set validity restriction
    vs::StartAndEndValidity start_and_end;
    start_and_end.start_validity = vs::convert_time32(time_now - std::chrono::hours(1));
    start_and_end.end_validity = vs::convert_time32(time_now + std::chrono::hours(24 * validity_days));
    certificate.validity_restriction.push_back(start_and_end);

    std::cout << "Signing certificate... ";

    // set signature
    vanetza::ByteBuffer data_buffer = vs::convert_for_signing(certificate);
    certificate.signature = crypto_backend.sign_data(key_pair.private_key, data_buffer);

    std::cout << "OK" << std::endl;

    std::cout << "Writing certificate to '" << output << "'... ";

    std::ofstream dest;
    dest.open(output.c_str(), std::ios::out | std::ios::binary);

    vanetza::OutputArchive archive(dest);
    vs::serialize(archive, certificate);

    std::cout << "OK" << std::endl;

    return 0;
}
