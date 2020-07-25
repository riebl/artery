#ifndef ARTERY_GEONETPACKET_H_OT36RUH0
#define ARTERY_GEONETPACKET_H_OT36RUH0

#include <vanetza/net/packet_variant.hpp>
#include <omnetpp/cpacket.h>
#include <memory>

namespace artery
{

class GeoNetPacket : public omnetpp::cPacket
{
    public:
        using omnetpp::cPacket::cPacket;

        GeoNetPacket() = default;
        GeoNetPacket(const GeoNetPacket&);
        GeoNetPacket& operator=(const GeoNetPacket&);
        GeoNetPacket(GeoNetPacket&&) = default;
        GeoNetPacket& operator=(GeoNetPacket&&) = default;

        void setPayload(std::unique_ptr<vanetza::PacketVariant>);
        void setPayload(std::unique_ptr<vanetza::CohesivePacket>);
        void setPayload(std::unique_ptr<vanetza::ChunkPacket>);
        const vanetza::PacketVariant& getPayload() const;
        bool hasPayload() const;
        std::unique_ptr<vanetza::PacketVariant> extractPayload() &&;

        int64_t getBitLength() const override;
        omnetpp::cPacket* dup() const override;

    private:
        std::unique_ptr<vanetza::PacketVariant> mPayload;
};

} // namespace artery

#endif /* ARTERY_GEONETPACKET_H_OT36RUH0 */

