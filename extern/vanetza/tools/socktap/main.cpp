#include "ethernet_device.hpp"
#include "gps_position_provider.hpp"
#include "hello_application.hpp"
#include "router_context.hpp"
#include "time_trigger.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/generic/raw_protocol.hpp>
#include <boost/program_options.hpp>
#include <cryptopp/eccrypto.h>
#include <cryptopp/files.h>
#include <cryptopp/oids.h>
#include <fstream>
#include <iostream>
#include <vanetza/security/default_certificate_validator.hpp>
#include <vanetza/security/naive_certificate_provider.hpp>
#include <vanetza/security/null_certificate_provider.hpp>
#include <vanetza/security/null_certificate_validator.hpp>
#include <vanetza/security/static_certificate_provider.hpp>
#include <vanetza/security/security_entity.hpp>
#include <vanetza/security/trust_store.hpp>

namespace asio = boost::asio;
namespace gn = vanetza::geonet;
namespace po = boost::program_options;
using namespace vanetza;

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

int main(int argc, const char** argv)
{
    po::options_description options("Allowed options");
    options.add_options()
        ("help", "Print out available options.")
        ("interface,i", po::value<std::string>()->default_value("lo"), "Network interface to use.")
        ("mac-address", po::value<std::string>(), "Override the network interface's MAC address.")
        ("security", po::value<std::string>()->default_value("off"), "Security profile to use.")
        ("gpsd-host", po::value<std::string>()->default_value(gpsd::shared_memory), "gpsd's server hostname")
        ("gpsd-port", po::value<std::string>()->default_value(gpsd::default_port), "gpsd's listening port")
        ("require-gnss-fix", "suppress transmissions while GNSS position fix is missing")
    ;

    po::positional_options_description positional_options;
    positional_options.add("interface", 1);

    po::variables_map vm;

    try {
        po::store(
            po::command_line_parser(argc, argv)
                .options(options)
                .positional(positional_options)
                .run(),
            vm
        );
        po::notify(vm);
    } catch (po::error& e) {
        std::cerr << "ERROR: " << e.what() << "\n\n";
        std::cerr << options << "\n";
        return 1;
    }

    if (vm.count("help")) {
        std::cout << options << "\n";
        return 1;
    }

    try {
        asio::io_service io_service;
        TimeTrigger trigger(io_service);

        const char* device_name = vm["interface"].as<std::string>().c_str();
        EthernetDevice device(device_name);
        vanetza::MacAddress mac_address = device.address();

        if (vm.count("mac-address")) {
            std::cout << "Using MAC address: " << vm["mac-address"].as<std::string>() << ".\n";

            if (!parse_mac_address(vm["mac-address"].as<std::string>().c_str(), mac_address)) {
                std::cerr << "The specified MAC address is invalid." << "\n";
                return 1;
            }
        }

        asio::generic::raw_protocol raw_protocol(AF_PACKET, gn::ether_type.net());
        asio::generic::raw_protocol::socket raw_socket(io_service, raw_protocol);
        raw_socket.bind(device.endpoint(AF_PACKET));

        auto signal_handler = [&io_service](const boost::system::error_code& ec, int signal_number) {
            if (!ec) {
                std::cout << "Termination requested." << std::endl;
                io_service.stop();
            }
        };
        asio::signal_set signals(io_service, SIGINT, SIGTERM);
        signals.async_wait(signal_handler);

        // configure management information base
        // TODO: make more MIB options configurable by command line flags
        gn::MIB mib;
        mib.itsGnLocalGnAddr.mid(mac_address);
        mib.itsGnLocalGnAddr.is_manually_configured(true);
        mib.itsGnLocalAddrConfMethod = geonet::AddrConfMethod::MANAGED;

        // We always use the same ceritificate manager and crypto services for now.
        // If itsGnSecurity is false, no signing will be performed, but receiving of signed messages works as expected.
        auto certificate_provider = std::unique_ptr<vanetza::security::CertificateProvider> { new vanetza::security::NaiveCertificateProvider(trigger.runtime().now()) };
        auto certificate_validator = std::unique_ptr<vanetza::security::CertificateValidator> { new vanetza::security::NullCertificateValidator() };
        auto crypto_backend = security::create_backend("default");

        std::vector<vanetza::security::Certificate> trusted_roots;
        vanetza::security::TrustStore* trust_store;

        const std::string& security_option = vm["security"].as<std::string>();
        if (security_option == "off") {
            mib.itsGnSecurity = false;
        } else if (security_option == "naive") {
            mib.itsGnSecurity = true;
        } else if (security_option == "null") {
            mib.itsGnSecurity = true;
            certificate_provider = std::unique_ptr<vanetza::security::CertificateProvider> { new vanetza::security::NullCertificateProvider() };
            crypto_backend = security::create_backend("Null");
        } else if (security_option == "default") {
            CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey ticket_key;
            CryptoPP::FileSource ticket_key_file("ticket.key", true);
            ticket_key.Load(ticket_key_file);

            auto authorization_ticket_key = to_keypair(ticket_key);

            vanetza::security::Certificate sign_cert;

            std::ifstream sign_cert_src;
            sign_cert_src.open("aa.cert", std::ios::in | std::ios::binary);

            vanetza::InputArchive sign_cert_archive(sign_cert_src);
            vanetza::security::deserialize(sign_cert_archive, sign_cert);

            vanetza::security::Certificate authorization_ticket;

            std::ifstream authorization_ticket_src;
            authorization_ticket_src.open("ticket.cert", std::ios::in | std::ios::binary);

            vanetza::InputArchive authorization_ticket_archive(authorization_ticket_src);
            vanetza::security::deserialize(authorization_ticket_archive, authorization_ticket);

            trusted_roots.push_back(sign_cert);
            trust_store = new vanetza::security::TrustStore(trusted_roots);

            mib.itsGnSecurity = true;
            certificate_provider = std::unique_ptr<vanetza::security::CertificateProvider> { new vanetza::security::StaticCertificateProvider(authorization_ticket, authorization_ticket_key) };
            certificate_validator = std::unique_ptr<vanetza::security::CertificateValidator> { new vanetza::security::DefaultCertificateValidator(trigger.runtime().now(), *trust_store) };
        } else {
            std::cerr << "Invalid security option '" << security_option << "', falling back to 'off'." << "\n";
            mib.itsGnSecurity = false;
        }

        security::SignService sign_service = straight_sign_service(trigger.runtime(), *certificate_provider, *crypto_backend);
        security::VerifyService verify_service = straight_verify_service(trigger.runtime(), *certificate_validator, *crypto_backend);

        GpsPositionProvider positioning(vm["gpsd-host"].as<std::string>(), vm["gpsd-port"].as<std::string>());
        security::SecurityEntity security_entity(sign_service, verify_service);
        RouterContext context(raw_socket, mib, trigger, positioning, security_entity);
        context.require_position_fix(vm.count("require-gnss-fix") > 0);

        asio::steady_timer hello_timer(io_service);
        HelloApplication hello_app(hello_timer);
        context.enable(&hello_app);

        io_service.run();
    } catch (GpsPositionProvider::gps_error& e) {
        std::cerr << "Exit because of GPS error: " << e.what() << std::endl;
        return 1;
    } catch (std::exception& e) {
        std::cerr << "Exit: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
