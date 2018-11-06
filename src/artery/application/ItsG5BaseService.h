//
// Copyright (C) 2014 Raphael Riebl <raphael.riebl@thi.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef ARTERY_ITSG5BASESERVICE_H_
#define ARTERY_ITSG5BASESERVICE_H_

#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <vanetza/btp/data_interface.hpp>
#include <vanetza/btp/data_request.hpp>
#include "artery/application/Facilities.h"
#include "artery/application/IndicationInterface.h"
#include "artery/application/Middleware.h"
#include "artery/application/NetworkInterface.h"
#include "artery/application/TransportDescriptor.h"
#include <set>

namespace artery
{

class ItsG5BaseService :
	public omnetpp::cSimpleModule, public omnetpp::cListener,
	public IndicationInterface
{
	public:
		using port_type = PortNumber; /*< deprecated type alias, use PortNumber */

		ItsG5BaseService();
		virtual ~ItsG5BaseService();

		/**
		 * Determine if this service expects to receive any ITS messages.
		 *
		 * Middleware will throw an error if no listening descriptors are configured for
		 * a service returning true by this method.
		 *
		 * \return true by default
		 */
		virtual bool requiresListener() const;

		/**
		 * Periodic service trigger.
		 *
		 * Middleware will call this method periodically as configured by its update interval.
		 */
		virtual void trigger();

		/**
		 * Add listening transport descriptor (channel + BTP port).
		 *
		 * Middleware will call this method for each descriptor configured for this service.
		 * \param td configured transport descriptor
		 */
		void addTransportDescriptor(const TransportDescriptor& td);

		/**
		 * Get set of all transport descriptor of this service
		 *
		 * \return transport descriptors
		 */
		const std::set<TransportDescriptor>& getTransportDescriptors() const { return m_listeners; }

	protected:
		void initialize() override;
		void finish() override;
		void request(const vanetza::btp::DataRequestB&, std::unique_ptr<vanetza::DownPacket>, const NetworkInterface* = nullptr);
		void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>, const NetworkInterface&) override;
		virtual void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>);
		Facilities& getFacilities();
		const Facilities& getFacilities() const;
		PortNumber getPortNumber(ChannelNumber = channel::CCH) const;
		std::set<TransportDescriptor> getListeningDescriptors() const;
		omnetpp::cModule* findHost();
		void subscribe(const omnetpp::simsignal_t&);
		void unsubscribe(const omnetpp::simsignal_t&);

	private:
		Middleware* m_middleware;
		std::set<TransportDescriptor> m_listeners;
};

} // namespace artery

#endif /* ARTERY_ITSG5BASESERVICE_H_ */
