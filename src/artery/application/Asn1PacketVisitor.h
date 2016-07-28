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

#ifndef __ARTERY_ASN1PACKETVISITOR_H_
#define __ARTERY_ASN1PACKETVISITOR_H_

#include <vanetza/common/byte_buffer_convertible.hpp>
#include <vanetza/net/chunk_packet.hpp>
#include <vanetza/net/cohesive_packet.hpp>
#include <vanetza/net/osi_layer.hpp>
#include <boost/variant/static_visitor.hpp>
#include <omnetpp/cexception.h>

template<class T>
struct Asn1PacketVisitor : public boost::static_visitor<const T*>
{
    const T* operator()(vanetza::CohesivePacket& packet)
    {
        throw omnetpp::cRuntimeError("ASN.1 packet deserialization is not yet implemented");
        return nullptr;
    }

    const T* operator()(vanetza::ChunkPacket& packet)
    {
        typedef vanetza::convertible::byte_buffer byte_buffer;
        typedef vanetza::convertible::byte_buffer_impl<T> byte_buffer_impl;

        byte_buffer* ptr = packet[vanetza::OsiLayer::Application].ptr();
        auto impl = dynamic_cast<byte_buffer_impl*>(ptr);
        if (impl) {
            shared_wrapper = impl->wrapper();
            return shared_wrapper.get();
        } else {
            throw omnetpp::cRuntimeError("ChunkPacket doesn't contain requested ASN.1 structure");
            return nullptr;
        }
    }

    std::shared_ptr<T> shared_wrapper;
};

#endif /* __ARTERY_ASN1PACKETVISITOR_H_ */
