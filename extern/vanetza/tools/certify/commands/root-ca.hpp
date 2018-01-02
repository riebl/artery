#ifndef CERTIFY_COMMANDS_ROOT_CA_HPP
#define CERTIFY_COMMANDS_ROOT_CA_HPP

#include "command.hpp"

class RootCaCommand : public Command
{
public:
    void parse(const std::vector<std::string>&) override;
    int execute() override;

private:
    std::string cert_key;
    std::string output;
    std::string subject_name;
    int validity_days;
};

#endif /* CERTIFY_COMMANDS_ROOT_CA_HPP */
