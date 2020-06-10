#include "artery/networking/GeoNetPacket.h"

Register_Class(artery::GeoNetPacket)

namespace artery
{

GeoNetPacket::GeoNetPacket(const GeoNetPacket& other)
{
    *this = other;
}

GeoNetPacket& GeoNetPacket::operator=(const GeoNetPacket& other)
{
    if (&other != this) {
        cPacket::operator=(other);
        if (other.mPayload) {
            mPayload.reset(new vanetza::PacketVariant(*other.mPayload));
        } else {
            mPayload.reset();
        }
    }
    return *this;
}

void GeoNetPacket::setPayload(std::unique_ptr<vanetza::PacketVariant> payload)
{
    mPayload = std::move(payload);
}

void GeoNetPacket::setPayload(std::unique_ptr<vanetza::CohesivePacket> payload)
{
    if (payload) {
        if (mPayload) {
            *mPayload = std::move(*payload);
        } else {
            mPayload.reset(new vanetza::PacketVariant(std::move(*payload)));
        }
    } else {
        mPayload.reset();
    }
}

void GeoNetPacket::setPayload(std::unique_ptr<vanetza::ChunkPacket> payload)
{
    if (payload) {
        if (mPayload) {
            *mPayload = std::move(*payload);
        } else {
            mPayload.reset(new vanetza::PacketVariant(std::move(*payload)));
        }
    } else {
        mPayload.reset();
    }
}

const vanetza::PacketVariant& GeoNetPacket::getPayload() const
{
    if (!mPayload) {
        throw omnetpp::cRuntimeError("No payload assigned to GeoNetPacket");
    }
    return *mPayload;
}

bool GeoNetPacket::hasPayload() const
{
    return mPayload != nullptr;
}

std::unique_ptr<vanetza::PacketVariant> GeoNetPacket::extractPayload() &&
{
    return std::move(mPayload);
}

int64_t GeoNetPacket::getBitLength() const
{
    int64_t length = omnetpp::cPacket::getBitLength();
    if (mPayload) {
        length += size(*mPayload) * 8;
    }
    return length;
}

omnetpp::cPacket* GeoNetPacket::dup() const
{
    return new GeoNetPacket(*this);
}

} // namespace artery
