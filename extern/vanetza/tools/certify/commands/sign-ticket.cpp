#include "keyio.hpp"
#include "sign-ticket.hpp"
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
#include <vanetza/security/its_aid.hpp>
#include <vanetza/security/subject_attribute.hpp>
#include <vanetza/security/subject_info.hpp>

namespace po = boost::program_options;
using namespace CryptoPP;
using namespace vanetza;
using namespace security;

void SignTicketCommand::parse(const std::vector<std::string>& opts)
{
    po::options_description desc("auth-ca options");
    desc.add_options()
        ("output", po::value<std::string>(&output)->required(), "Output file.")
        ("sign-key", po::value<std::string>(&sign_key)->required(), "Private key file of the signer.")
        ("sign-cert", po::value<std::string>(&sign_cert)->required(), "Private certificate file of the signer.")
        ("subject-key", po::value<std::string>(&subject_key)->required(), "Private key file to issue the certificate for.")
        ("days", po::value<int>(&validity_days)->default_value(7), "Validity in days.")
    ;

    po::positional_options_description pos;
    pos.add("output", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(opts).options(desc).positional(pos).run(), vm);
    po::notify(vm);
}

int SignTicketCommand::execute()
{
    std::cout << "Loading keys... ";

    AutoSeededRandomPool rng;
    BackendCryptoPP crypto_backend;

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

    Certificate loaded_sign_cert;

    std::ifstream src;
    src.open(sign_cert.c_str(), std::ios::in | std::ios::binary);

    InputArchive iarchive(src);
    deserialize(iarchive, loaded_sign_cert);

    auto sign_key_pair = to_keypair(loaded_sign_key);
    auto subject_key_pair = to_keypair(loaded_subject_key);
    auto time_now = Clock::at(boost::posix_time::microsec_clock::universal_time());

    Certificate certificate;

    std::list<IntX> certificate_aids;
    certificate_aids.push_back(itsAidCa);
    certificate_aids.push_back(itsAidDen);

    std::list<ItsAidSsp> certificate_ssp;
    // see  ETSI EN 302 637-2 V1.3.1 (2014-09)
    ItsAidSsp certificate_ssp_ca;
    certificate_ssp_ca.its_aid = itsAidCa;
    certificate_ssp_ca.service_specific_permissions = ByteBuffer({ 1, 0, 0 }); // no special permissions
    certificate_ssp.push_back(certificate_ssp_ca);

    certificate.signer_info = calculate_hash(loaded_sign_cert);
    certificate.subject_info.subject_type = SubjectType::Authorization_Ticket;
    certificate.subject_attributes.push_back(SubjectAssurance(0x00));
    certificate.subject_attributes.push_back(certificate_ssp);
    certificate.subject_attributes.push_back(certificate_aids);

    Uncompressed coordinates;
    coordinates.x.assign(subject_key_pair.public_key.x.begin(), subject_key_pair.public_key.x.end());
    coordinates.y.assign(subject_key_pair.public_key.y.begin(), subject_key_pair.public_key.y.end());
    EccPoint ecc_point = coordinates;
    ecdsa_nistp256_with_sha256 ecdsa;
    ecdsa.public_key = ecc_point;
    VerificationKey verification_key;
    verification_key.key = ecdsa;
    certificate.subject_attributes.push_back(verification_key);

    StartAndEndValidity start_and_end;
    start_and_end.start_validity = convert_time32(time_now - std::chrono::hours(1));
    start_and_end.end_validity = convert_time32(time_now + std::chrono::hours(24 * validity_days));
    certificate.validity_restriction.push_back(start_and_end);

    std::cout << "Signing certificate... ";

    ByteBuffer data_buffer = convert_for_signing(certificate);
    certificate.signature = crypto_backend.sign_data(sign_key_pair.private_key, data_buffer);

    std::cout << "OK" << std::endl;

    std::cout << "Writing certificate to '" << output << "'... ";

    std::ofstream dest;
    dest.open(output.c_str(), std::ios::out | std::ios::binary);

    OutputArchive archive(dest);
    serialize(archive, certificate);

    std::cout << "OK" << std::endl;

    return 0;
}
