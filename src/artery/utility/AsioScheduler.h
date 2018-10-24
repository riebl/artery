//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef ARTERY_ASIOSCHEDULER_H_
#define ARTERY_ASIOSCHEDULER_H_

#include <omnetpp/cmodule.h>
#include <omnetpp/cscheduler.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <memory>

namespace artery
{

class AsioTask;

class AsioScheduler : public omnetpp::cScheduler
{
	public:
		AsioScheduler();
		virtual std::string info() const override;
		std::unique_ptr<AsioTask> createTask(omnetpp::cModule&);
		void cancelTask(AsioTask*);
		void processTask(AsioTask*);

	protected:
		virtual omnetpp::cEvent* guessNextEvent() override;
		virtual omnetpp::cEvent* takeNextEvent() override;
		virtual void putBackEvent(omnetpp::cEvent*) override;

		virtual void endRun() override;
		virtual void startRun() override;
		virtual void executionResumed() override;

	private:
		void handleTask(AsioTask*, const boost::system::error_code&, std::size_t bytes);
		void handleTimer(const boost::system::error_code&);
		void setTimer();

		enum class FluxState {
			PAUSED, DWADLING, SYNC
		};

		boost::asio::io_service m_service;
		boost::asio::io_service::work m_work;
		boost::asio::steady_timer m_timer;
		std::chrono::steady_clock::time_point m_reference;
		std::chrono::steady_clock::time_point m_run_until;
		FluxState m_state;
};

} // namespace artery

#endif /* ARTERY_ASIOSCHEDULER_H_ */
