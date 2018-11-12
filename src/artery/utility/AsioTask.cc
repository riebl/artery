#include "AsioScheduler.h"
#include "AsioTask.h"
#include <boost/asio/write.hpp>

namespace artery
{

AsioTask::AsioTask(AsioScheduler& scheduler, boost::asio::ip::tcp::socket socket, omnetpp::cModule& mod) :
	m_scheduler(scheduler), m_socket(std::move(socket)), m_message(new AsioData("Asio Data")), m_module(mod)
{
}

AsioTask::~AsioTask()
{
	m_scheduler.cancelTask(this);
}

void AsioTask::write(boost::asio::const_buffer buf)
{
	boost::asio::write(m_socket, boost::asio::buffer(buf));
}

void AsioTask::connect(boost::asio::ip::tcp::endpoint ep, bool tcp_no_delay)
{
	m_socket.connect(ep);
	m_socket.set_option(boost::asio::ip::tcp::no_delay(tcp_no_delay));
	m_scheduler.processTask(this);
}

void AsioTask::handleNext()
{
	m_scheduler.processTask(this);
}

} // namespace artery
