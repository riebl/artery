/* -*- mode:c++ -*- ********************************************************
 * file:        BasicSinkRouting.cc
 *
 * author:      Tom Parker
 *
 * copyright:   (C) 2006 Parallel and Distributed Systems Group (PDS) at
 *              Technische Universiteit Delft, The Netherlands.
 *
 *              This program is free software; you can redistribute it 
 *              and/or modify it under the terms of the GNU General Public 
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later 
 *              version.
 *              For further information see file COPYING 
 *              in the top level directory
 ***************************************************************************
 * part of:     wsn-specific modules
 * description: network layer: basic source-to-sink routing
 ***************************************************************************/


#include "BasicSinkRouting.h"
#include "NetwControlInfo.h"
#include "MacControlInfo.h"
#include "NicControlType.h"

Define_Module(BasicSinkRouting);

void BasicSinkRouting::toNetwork(NetwPkt * out)
{
	if (msgBusy)
	{
		msgQueue->push(out);
		EV << "Local link is busy, queuing for future send" << endl;
	}
	else if (out->getDestAddr() == SINK_ADDRESS && !setNextHop(out))
	{
		msgQueue->push(out);
		EV << "Msg for sink and we don't have any!" << endl;
	}
	else
	{
		msgBusy = true;
		EV << "Pushing over local link" << endl;
		sendDown(out);
	}
}

NetwPkt *BasicSinkRouting::buildSink(SinkInfo * sink, int from)
{
	NetwPkt *pkt = buildPkt(SINK_BCAST, L3BROADCAST, "sink");
	if (sink == NULL)
	{
		sink = new SinkInfo();
		sink->setSinkId(myNetwAddr);
		sink->setParent(-1);
		sink->setCost(0);
	}
	else
	{
		sink = check_and_cast < SinkInfo * >(sink->dup());
		sink->setCost(sink->getCost() + 1);
		sink->setParent(from);
	}
	pkt->encapsulate(sink);
	return pkt;
}

void BasicSinkRouting::initialize(int stage)
{
	QueuedRouting::initialize(stage);
	Timer::init(this);

	if (stage == 0)
	{
		sinks = new std::map < int, SinkInfo * >();
	}
	else if (stage == 1)
	{
		cModule *node = getNode();
		if (node->hasPar("isSink") && node->par("isSink").boolValue())
		{
			isSink = true;
			EV << "Sink node, broadcasting\n";
			NetwPkt *out = buildSink();
			(*sinks)[myNetwAddr] = check_and_cast < SinkInfo * >(out->getEncapsulatedMsg()->dup());
			msgQueue->push(out);
			setTimer(0, 0.001);
		}
		else
			isSink = false;
	}
}

BasicSinkRouting::~BasicSinkRouting()
{
	for (std::map < int, SinkInfo * >::iterator si = sinks->begin(); si != sinks->end(); si++)
	{
		delete(*si).second;
	}
	delete sinks;
}

void BasicSinkRouting::finish()
{
	QueuedRouting::finish();
	printSinks();
}

void BasicSinkRouting::printSinks()
{
	for (std::map < int, SinkInfo * >::iterator si = sinks->begin(); si != sinks->end(); si++)
	{
		SinkInfo *s = (*si).second;
		EV << "Sink " << s->getSinkId() << " is findable via parent " << s->getParent() << " (macAddr = " << (s->getParent() == -1 ? -1 : arp->getMacAddr(s->getParent())) << ") with cost " << s->getCost() << endl;
	}
}

bool BasicSinkRouting::setNextHop(NetwPkt * pkt)
{
	int macAddr;
	delete pkt->removeControlInfo();
	if (pkt->getDestAddr() != SINK_ADDRESS)
		opp_error("non-sink packet!");
	if (sinks->size() > 0)
	{
		printSinks();
		macAddr = arp->getMacAddr(sinks->begin()->second->getParent());
		EV << "setNextHop: nHop=SINK_ADDRESS -> sending to sink via parent " << macAddr << endl;
	}
	else
	{
		// don't know where to send this yet
		EV << "setNextHop: nHop=SINK_ADDRESS -> need to find a sink to send this to" << endl;
		macAddr = -2;
	}
	pkt->setControlInfo(new MacControlInfo(macAddr));
	return macAddr != -2;
}

/**
 * Redefine this function if you want to process messages from lower
 * layers before they are forwarded to upper layers
 *
 *
 * If you want to forward the message to upper layers please use
 * @ref sendUp which will take care of decapsulation and thelike
 **/
void BasicSinkRouting::handleLowerMsg(cMessage * msg)
{
	NetwPkt *m = check_and_cast < NetwPkt * >(msg);
	EV << "handling packet from " << m->getSrcAddr() << endl;
	EV << "Incoming type is " << m->getKind() << endl;
	switch (m->getKind())
	{
		case UPPER_TYPE:
			{
				if (m->getDestAddr() == SINK_ADDRESS && !isSink && (!hasPar("autoForward") || par("autoForward").boolValue()))
				{
					//printSinks();
					EV << "Sink packet going through" << endl;
					toNetwork(m);
				}
				else
				{
					EV << "msg.destAddr = " << m->getDestAddr() 
					   << " and isSink = " << isSink
					   << ", so I'm sending up.\n";
					sendUp(decapsMsg(m));
				}
				break;
			}
		case SINK_BCAST:
			{
				int srcAddr = m->getSrcAddr();
				SinkInfo *si = check_and_cast < SinkInfo * >(decapsMsg(m));
				EV << "got new sink info " << si->getSinkId() << " from node " << srcAddr << " with cost " << si->getCost() << endl;
				std::map < int, SinkInfo * >::iterator i = sinks->find(si->getSinkId());
				if (i != sinks->end())
				{
					SinkInfo *old = (*i).second;
					if (old->getCost() <= si->getCost() + 1)
					{
						EV << "new sink is too expensive. Have " << old->getParent() << " with cost " << old->getCost() << endl;
						delete si;
						break;
					}
					else
					{
						EV << "new sink is cheap! Have " << old->getParent() << " with cost " << old->getCost() << endl;
						sinks->erase(i);
						delete old;
					}
				}
				else
					EV << "Brand new sink!" << endl;
				NetwPkt *out = buildSink(si, srcAddr);
				SinkInfo *ns = check_and_cast < SinkInfo * >(si->dup());
				ns->setParent(srcAddr);
				ns->setCost(ns->getCost() + 1);
				(*sinks)[ns->getSinkId()] = ns;
				EV << "got new sink " << ns->getSinkId() << " and my parent node is " << ns->getParent() << endl;

				if (sinks->size() > 1)
					opp_error("Panic! Got more than 1 sinks... we can't handle that yet");

				toNetwork(out);
				delete si;
				break;
			}

		default:
			opp_error("one of mine, but not handling");
			break;
	}
}

/**
 * Redefine this function if you want to process messages from upper
 * layers before they are send to lower layers.
 *
 * For the BasicSinkRouting we just use the destAddr of the network
 * message as a nextHop
 *
 * To forward the message to lower layers after processing it please
 * use @ref toNetwork. It will take care of anything needed
 **/

void BasicSinkRouting::handleUpperMsg(cMessage * msg)
{
	NetwControlInfo *cInfo = dynamic_cast < NetwControlInfo * >(msg->getControlInfo());
	if (isSink && (cInfo==NULL || cInfo->getNetwAddr() != L3BROADCAST))
	{
		EV << "D'oh. I'm the sink" << endl;
		sendUp(msg);
	}
	else
	{
		EV << "Sending upper layer packet to sink" << endl;
		toNetwork(encapsMsg(msg));
	}
}


void BasicSinkRouting::handleTimer(unsigned int count)
{
	sendQueued();
}

int BasicSinkRouting::specialNetwAddress(int netwAddr)
{
	if (netwAddr == SINK_ADDRESS) {
		return netwAddr;
	} else {
		opp_error("Don't know special address %d",netwAddr);
		return -1;
	}
}	   

int BasicSinkRouting::specialMACAddress(int netwAddr)
{
	if (netwAddr == SINK_ADDRESS) {
		if (sinks->begin() != sinks->end()) {
			return arp->getMacAddr(sinks->begin()->second->getParent());
		} else {
			EV << "Route not initialized (yet), send as L2BROADCAST" << endl;
			return L2BROADCAST;
		}
	} else {
		opp_error("Don't know special address %d",netwAddr);
		return -1;
	}
}