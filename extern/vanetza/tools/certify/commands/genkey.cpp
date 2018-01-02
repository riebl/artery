#include "genkey.hpp"
#include "keyio.hpp"
#include <boost/program_options.hpp>
#include <cryptopp/eccrypto.h>
#include <cryptopp/oids.h>
#include <cryptopp/osrng.h>
#include <cryptopp/queue.h>
#include <cryptopp/sha.h>
#include <iostream>
#include <stdexcept>

namespace po = boost::program_options;
using namespace CryptoPP;

void GenkeyCommand::parse(const std::vector<std::string>& opts)
{
    po::options_description desc("genkey options");
    desc.add_options()
        ("output", po::value<std::string>(&output)->required(), "Output file.")
    ;

    po::positional_options_description pos;
    pos.add("output", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(opts).options(desc).positional(pos).run(), vm);
    po::notify(vm);
}

int GenkeyCommand::execute()
{
    std::cout << "Generating key..." << std::endl;

    AutoSeededRandomPool rng;
    OID oid(CryptoPP::ASN1::secp256r1());
    ECDSA<ECP, SHA256>::PrivateKey private_key;
    private_key.Initialize(rng, oid);

    if (!private_key.Validate(rng, 3)) {
        throw std::runtime_error("Private key validation failed");
    }

    ByteQueue queue;
    private_key.Save(queue);
    save_key(output, queue);

    return 0;
}
