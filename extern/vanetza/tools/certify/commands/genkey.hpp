#ifndef CERTIFY_COMMANDS_GENKEY_HPP
#define CERTIFY_COMMANDS_GENKEY_HPP

#include "command.hpp"

class GenkeyCommand : public Command
{
public:
    void parse(const std::vector<std::string>&) override;
    int execute() override;

private:
    std::string output;
};

#endif /* CERTIFY_COMMANDS_GENKEY_HPP */
