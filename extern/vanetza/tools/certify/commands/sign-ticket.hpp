#ifndef CERTIFY_COMMANDS_SIGN_TICKET_HPP
#define CERTIFY_COMMANDS_SIGN_TICKET_HPP

#include "command.hpp"

class SignTicketCommand : public Command
{
public:
    void parse(const std::vector<std::string>&) override;
    int execute() override;

private:
    std::string output;
    std::string sign_key;
    std::string sign_cert;
    std::string subject_key;
    int validity_days;
};

#endif /* CERTIFY_COMMANDS_SIGN_TICKET_HPP */
