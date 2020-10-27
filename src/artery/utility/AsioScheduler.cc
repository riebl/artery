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

#include "AsioScheduler.h"
#include "AsioTask.h"
#include <chrono>
#include <functional>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/steady_timer.hpp>

namespace artery
{

Register_Class(AsioScheduler)

using namespace omnetpp;

template<typename PERIOD>
struct clock_resolution { static const SimTimeUnit unit; };

template<>
const SimTimeUnit clock_resolution<std::milli>::unit = SIMTIME_MS;

template<>
const SimTimeUnit clock_resolution<std::micro>::unit = SIMTIME_US;

template<>
const SimTimeUnit clock_resolution<std::nano>::unit = SIMTIME_NS;

constexpr SimTimeUnit steady_clock_resolution()
{
	return clock_resolution<std::chrono::steady_clock::period>::unit;
}

std::chrono::steady_clock::duration steady_clock_duration(const SimTime t)
{
	return std::chrono::steady_clock::duration { t.inUnit(steady_clock_resolution()) };
}


AsioScheduler::AsioScheduler() : m_work(m_service), m_timer(m_service), m_state(FluxState::PAUSED)
{
}

std::string AsioScheduler::info() const
{
	return std::string("Asio Scheduler (") + SimTime(1, steady_clock_resolution()).str() + " resolution)";
}

cEvent* AsioScheduler::guessNextEvent()
{
	return sim->getFES()->peekFirst();
}

cEvent* AsioScheduler::takeNextEvent()
{
	while (true) {
		cEvent* event = sim->getFES()->peekFirst();
		if (event) {
			if (event->isStale()) {
				cEvent* tmp = sim->getFES()->removeFirst();
				ASSERT(tmp == event);
				delete tmp;
			} else {
				m_run_until = m_reference + steady_clock_duration(event->getArrivalTime());
				try {
					ASSERT(!m_service.stopped());
					setTimer();
					while (m_state == FluxState::DWADLING) {
						m_service.run_one();
					}
					m_timer.cancel();
					m_service.poll();
				} catch (boost::system::system_error& e) {
					cRuntimeError("AsioScheduler IO error: %s", e.what());
				}

				if (m_state == FluxState::SYNC) {
					return sim->getFES()->removeFirst();
				} else {
					return nullptr;
				}
			}
		} else {
			throw cTerminationException(E_ENDEDOK);
		}
	}
}

void AsioScheduler::putBackEvent(cEvent* event)
{
	sim->getFES()->putBackFirst(event);
}

void AsioScheduler::startRun()
{
	m_state = FluxState::SYNC;
	if (m_service.stopped()) {
		m_service.reset();
	}
	m_reference = std::chrono::steady_clock::now();
}

void AsioScheduler::endRun()
{
	m_state = FluxState::PAUSED;
	m_service.stop();
}

void AsioScheduler::executionResumed()
{
	m_state = FluxState::SYNC;
	m_reference = std::chrono::steady_clock::now();
	m_reference -= steady_clock_duration(simTime());
}

void AsioScheduler::handleTimer(const boost::system::error_code& ec)
{
	if (getEnvir()->idle()) {
		m_state = FluxState::PAUSED;
	} else if (ec) {
		m_state = FluxState::SYNC;
	} else {
		setTimer();
	}
}

void AsioScheduler::setTimer()
{
	static const auto max_timer = std::chrono::milliseconds(100);
	const auto now = std::chrono::steady_clock::now();
	if (m_run_until > now) {
		m_state = FluxState::DWADLING;
		m_timer.expires_at(std::min(m_run_until, now + max_timer));
		m_timer.async_wait(std::bind(&AsioScheduler::handleTimer, this, std::placeholders::_1));
	} else {
		m_state = FluxState::SYNC;
	}
}

std::unique_ptr<AsioTask> AsioScheduler::createTask(cModule& mod)
{
	std::unique_ptr<AsioTask> result;
	boost::asio::ip::tcp::socket socket(m_service);
	result.reset(new AsioTask(*this, std::move(socket), mod));
	return result;
}

void AsioScheduler::cancelTask(AsioTask* task)
{
	if (task != nullptr) {
		sim->getFES()->remove(task->getDataMessage());
	}
}

void AsioScheduler::processTask(AsioTask* task)
{
	using namespace std::placeholders;
	auto& buffer = task->m_message->getBuffer();
	auto handler = std::bind(&AsioScheduler::handleTask, this, task, _1, _2);
	task->m_socket.async_read_some(boost::asio::buffer(buffer.data(), buffer.size()), handler);
}

void AsioScheduler::handleTask(AsioTask* task, const boost::system::error_code& ec, std::size_t bytes)
{
	if (!ec) {
		using namespace std::chrono;
		const auto arrival_clock = steady_clock::now() - m_reference;
		const SimTime arrival_simtime { arrival_clock.count(), steady_clock_resolution() };
		ASSERT(simTime() <= arrival_simtime);

		AsioData* msg = task->getDataMessage();
		msg->setLength(bytes);
		msg->setArrival(task->getDestinationModule()->getId(), -1, arrival_simtime);
		sim->getFES()->insert(msg);
	} else if (ec != boost::asio::error::operation_aborted) {
		throw cRuntimeError("AsioScheduler: Failed reading from socket: %s", ec.message().c_str());
	}
}

} // namespace artery
