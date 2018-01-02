#ifndef CERTIFY_COMMANDS_AUTH_CA_HPP
#define CERTIFY_COMMANDS_AUTH_CA_HPP

#include "command.hpp"

class AuthCaCommand : public Command
{
public:
    void parse(const std::vector<std::string>&) override;
    int execute() override;

private:
    std::string output;
    std::string sign_key;
    std::string sign_cert;
    std::string subject_key;
    std::string subject_name;
    int validity_days;
};

#endif /* CERTIFY_COMMANDS_AUTH_CA_HPP */
