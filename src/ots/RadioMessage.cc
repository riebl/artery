#include <ots/RadioMessage.h>
#include <sim0mqpp/counter_serialization.hpp>

namespace ots
{

omnetpp::cPacket* RadioMessage::dup() const
{
    return new RadioMessage(*this);
}

void RadioMessage::setSender(const std::string& id)
{
    mSender = id;
}

const std::string& RadioMessage::getSender() const
{
    return mSender;
}

void RadioMessage::setReceiver(const std::string& id)
{
    mReceiver = id;
}

const std::string& RadioMessage::getReceiver() const
{
    return mReceiver;
}

void RadioMessage::setPayload(std::vector<sim0mqpp::Any> payload)
{
    mPayload = std::move(payload);
}

void RadioMessage::appendPayload(const sim0mqpp::Any& element)
{
    mPayload.push_back(element);
}

const std::vector<sim0mqpp::Any>& RadioMessage::getPayload() const
{
    return mPayload;
}

std::size_t RadioMessage::getPayloadLength() const
{
    sim0mqpp::CounterSerializer output;
    for (const auto& elem : mPayload)
    {
        sim0mqpp::serialize(output, elem);
    }
    return output.counter();
}

} // namespace ots
