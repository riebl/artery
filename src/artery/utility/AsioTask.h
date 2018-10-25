#ifndef ARTERY_ASIOTASK_H_
#define ARTERY_ASIOTASK_H_

#include "artery/utility/AsioData_m.h"
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <omnetpp/cmodule.h>
#include <memory>

namespace artery
{

class AsioScheduler;

class AsioTask
{
	public:
		AsioTask(AsioScheduler&, boost::asio::ip::tcp::socket, omnetpp::cModule&);
		virtual ~AsioTask();
		void write(boost::asio::const_buffer);
		void connect(boost::asio::ip::tcp::endpoint, bool tcp_no_delay = false);
		void handleNext();
		AsioData* getDataMessage() { return m_message.get(); }
		const omnetpp::cModule* getDestinationModule() { return &m_module; }

	private:
		friend class AsioScheduler;

		AsioScheduler& m_scheduler;
		boost::asio::ip::tcp::socket m_socket;
		std::unique_ptr<AsioData> m_message;
		omnetpp::cModule& m_module;
};

} // namespace artery

#endif /* ARTERY_ASIOTASK_H_ */
