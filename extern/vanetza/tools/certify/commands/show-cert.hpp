#ifndef CERTIFY_COMMANDS_SHOW_CERT_HPP
#define CERTIFY_COMMANDS_SHOW_CERT_HPP

#include "command.hpp"

class ShowCertCommand : public Command
{
public:
    void parse(const std::vector<std::string>&) override;
    int execute() override;

private:
    std::string certificate_path;
};

#endif /* CERTIFY_COMMANDS_SHOW_CERT_HPP */
