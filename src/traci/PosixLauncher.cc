#include "traci/PosixLauncher.h"
#include <omnetpp/cconfiguration.h>
#include <cerrno>
#include <regex>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>

namespace traci
{

Define_Module(PosixLauncher)

PosixLauncher::PosixLauncher() : m_pid(0)
{
}

PosixLauncher::~PosixLauncher()
{
    kill();
}

void PosixLauncher::initialize()
{
    m_command = par("command").stringValue();
    m_sumocfg = par("sumocfg").stringValue();
    m_port = par("port");
    m_seed = par("seed");
}

void PosixLauncher::finish()
{
    kill();
}

ServerEndpoint PosixLauncher::launch()
{
    if (m_port == 0) {
        m_port = lookupPort();
    }

    ServerEndpoint endpoint;
    endpoint.hostname = "localhost";
    endpoint.port = m_port;
    endpoint.retry = true;

    m_pid = ::fork();
    if (m_pid < 0) {
        throw omnetpp::cRuntimeError("fork() failed: %s", std::strerror(errno));
    } else if (m_pid == 0) {
        if (::execl("/bin/sh", "sh", "-c", command().c_str(), NULL)  == -1) {
            throw omnetpp::cRuntimeError("Starting TraCI server failed: %s", std::strerror(errno));
        }
        ::_exit(1);
    }

    return endpoint;
}

void PosixLauncher::kill()
{
    if (m_pid != 0) {
        ::kill(m_pid, SIGINT);
        m_pid = 0;
    }

    ::waitpid(m_pid, NULL, 0);
}

std::string PosixLauncher::command()
{
    std::regex sumocfg("%SUMOCFG%");
    std::regex port("%PORT%");
    std::regex seed("%SEED%");
    std::regex run("%RUN%");

    const auto run_number = getSimulation()->getEnvir()->getConfigEx()->getVariable(CFGVAR_RUNNUMBER);

    std::string command = m_command;
    command = std::regex_replace(command, sumocfg, m_sumocfg);
    command = std::regex_replace(command, port, std::to_string(m_port));
    command = std::regex_replace(command, seed, std::to_string(m_seed));
    command = std::regex_replace(command, run, run_number);
    return command;
}

int PosixLauncher::lookupPort()
{
    int sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw omnetpp::cRuntimeError("socket() failed: %s", std::strerror(errno));
    }

    sockaddr_in sin;
    socklen_t sin_len = sizeof(sin);
    std::memset(&sin, 0, sin_len);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = 0;

    if (::bind(sock, (struct sockaddr*) &sin, sin_len) == -1) {
        throw omnetpp::cRuntimeError("bind() failed: %s", std::strerror(errno));
    }

    if (::getsockname(sock, (struct sockaddr*) &sin, &sin_len) == -1) {
        throw omnetpp::cRuntimeError("getsockname() failed: %s", std::strerror(errno));
    }

    ::close(sock);
    return ntohs(sin.sin_port);
}

} // namespace traci
