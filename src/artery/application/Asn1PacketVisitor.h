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

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/byte_buffer_convertible.hpp>
#include <vanetza/net/chunk_packet.hpp>
#include <vanetza/net/cohesive_packet.hpp>
#include <vanetza/net/osi_layer.hpp>
#include <boost/variant/static_visitor.hpp>
#include <omnetpp/clog.h>
#include <memory>
#include <typeinfo>

template<class T>
struct Asn1PacketVisitor : public boost::static_visitor<const T*>
{
    const T* operator()(vanetza::CohesivePacket& packet)
    {
        const auto range = packet[vanetza::OsiLayer::Application];
        vanetza::ByteBuffer buffer { range.begin(), range.end() };
        deserialize(buffer);
        return shared_wrapper.get();
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
            vanetza::ByteBuffer buffer;
            packet[vanetza::OsiLayer::Application].convert(buffer);
            deserialize(buffer);
            return shared_wrapper.get();
        }
    }

    void deserialize(const vanetza::ByteBuffer& buffer)
    {
        auto temp_wrapper = std::make_shared<T>();
        bool decoded = temp_wrapper->decode(buffer);
        if (decoded) {
            shared_wrapper = temp_wrapper;
        } else {
            using namespace omnetpp;
            const std::type_info& asn1_type = typeid(T);
            EV_ERROR << "Decoding of " << asn1_type.name() << " failed";
        }
    }

    std::shared_ptr<T> shared_wrapper;
};

#endif /* __ARTERY_ASN1PACKETVISITOR_H_ */
