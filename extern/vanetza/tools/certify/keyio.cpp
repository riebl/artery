#include "keyio.hpp"
#include <cryptopp/files.h>

void save_key(const std::string& filename, const CryptoPP::BufferedTransformation& bt)
{
    CryptoPP::FileSink file(filename.c_str());

    bt.CopyTo(file);
    file.MessageEnd();
}

void load_key(const std::string& filename, CryptoPP::BufferedTransformation& bt)
{
    CryptoPP::FileSource file(filename.c_str(), true);

    file.TransferTo(bt);
    bt.MessageEnd();
}

vanetza::security::ecdsa256::KeyPair to_keypair(const vanetza::security::BackendCryptoPP::PrivateKey& private_key)
{
    vanetza::security::ecdsa256::KeyPair kp;

    auto& private_exponent = private_key.GetPrivateExponent();
    private_exponent.Encode(kp.private_key.key.data(), kp.private_key.key.size());

    vanetza::security::BackendCryptoPP::PublicKey public_key;
    private_key.MakePublicKey(public_key);

    auto& public_element = public_key.GetPublicElement();
    public_element.x.Encode(kp.public_key.x.data(), kp.public_key.x.size());
    public_element.y.Encode(kp.public_key.y.data(), kp.public_key.y.size());

    return kp;
}
