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

namespace {

/**
 * Block process signals during object lifetime
 */
class BlockSignal
{
public:
    BlockSignal(std::initializer_list<int> block_signals);
    ~BlockSignal();

    // no copy
    BlockSignal(const BlockSignal&) = delete;
    BlockSignal& operator=(const BlockSignal&) = delete;

private:
    sigset_t block_mask;
};

BlockSignal::BlockSignal(std::initializer_list<int> block_signals)
{
    ::sigemptyset(&block_mask);
    for (int block_signal : block_signals) {
        ::sigaddset(&block_mask, block_signal);
    }
    ::pthread_sigmask(SIG_BLOCK, &block_mask, nullptr);
}

BlockSignal::~BlockSignal()
{
    ::pthread_sigmask(SIG_UNBLOCK, &block_mask, nullptr);
}

}


PosixLauncher::PosixLauncher() : m_pid(0)
{
}

PosixLauncher::~PosixLauncher()
{
    kill();
}

void PosixLauncher::initialize()
{
    m_executable = par("sumo").stringValue();
    m_command = par("command").stringValue();
    m_sumocfg = par("sumocfg").stringValue();
    m_extra_options = par("extraOptions").stringValue();
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

    // workaround: creates <resultdir> before executing SUMO (for logfile output)
    recordScalar("port", m_port);

    ServerEndpoint endpoint;
    endpoint.hostname = "localhost";
    endpoint.port = m_port;
    endpoint.retry = true;

    // temporarily block SIGINT during fork sequence
    BlockSignal block_sigint({ SIGINT });

    m_pid = ::fork();
    if (m_pid < 0) {
        throw omnetpp::cRuntimeError("fork() failed: %s", std::strerror(errno));
    } else if (m_pid == 0) {
        // ignore signal so SUMO does not quit when user interrupts in gdb
        ::signal(SIGINT, SIG_IGN);

        // sumo-gui resets SIGINT handler: move process to own process group
        ::setpgid(0, 0);

        if (::execl("/bin/sh", "sh", "-c", command().c_str(), NULL)  == -1) {
            throw omnetpp::cRuntimeError("Starting TraCI server failed: %s", std::strerror(errno));
        }
        ::_exit(1);
    } else {
        // race between parent and child (see setpgid RATIONALE)
        if (::setpgid(m_pid, m_pid) != 0 && errno != EACCES) {
            throw omnetpp::cRuntimeError("setpgid() failed: %s", std::strerror(errno));
        }
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
    std::regex executable("%SUMO%");
    std::regex sumocfg("%SUMOCFG%");
    std::regex port("%PORT%");
    std::regex seed("%SEED%");
    std::regex run("%RUN%");
    std::regex resultdir("%RESULTDIR%");

    const auto cfg = getSimulation()->getEnvir()->getConfigEx();
    const auto cfg_run_number = cfg->getVariable(CFGVAR_RUNNUMBER);
    const auto cfg_result_dir = cfg->getVariable(CFGVAR_RESULTDIR);

    std::string command = m_command;
    command = std::regex_replace(command, executable, m_executable);
    command = std::regex_replace(command, sumocfg, m_sumocfg);
    command = std::regex_replace(command, port, std::to_string(m_port));
    command = std::regex_replace(command, seed, std::to_string(m_seed));
    command = std::regex_replace(command, run, cfg_run_number);
    command = std::regex_replace(command, resultdir, cfg_result_dir);

    if (!m_extra_options.empty()) {
      command.append(1, ' ').append(m_extra_options);
    }

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
