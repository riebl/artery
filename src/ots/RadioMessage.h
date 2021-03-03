#ifndef OTS_RADIOMESSAGE_6C0B3EB6
#define OTS_RADIOMESSAGE_6C0B3EB6

#include <omnetpp/cpacket.h>
#include <sim0mqpp/any.hpp>
#include <string>
#include <vector>

namespace ots
{

class RadioMessage : public omnetpp::cPacket
{
public:
    using omnetpp::cPacket::cPacket;

    void setSender(const std::string&);
    const std::string& getSender() const;

    void setReceiver(const std::string&);
    const std::string& getReceiver() const;

    void setPayload(std::vector<sim0mqpp::Any>);
    void appendPayload(const sim0mqpp::Any&);
    const std::vector<sim0mqpp::Any>& getPayload() const;
    std::size_t getPayloadLength() const;

    omnetpp::cPacket* dup() const override;

private:
    std::string mSender;
    std::string mReceiver;
    std::vector<sim0mqpp::Any> mPayload;
};

} // namespace ots

#endif /* OTS_RADIOMESSAGE_6C0B3EB6 */
