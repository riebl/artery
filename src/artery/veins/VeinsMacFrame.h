#ifndef ARTERY_VEINSMACFRAME_H_6QLH2JU5
#define ARTERY_VEINSMACFRAME_H_6QLH2JU5

#include <veins/modules/messages/BaseFrame1609_4_m.h>

namespace artery
{

class VeinsMacFrame : public veins::BaseFrame1609_4
{
public:
    using Address = veins::LAddress::L2Type;

    void setSenderAddress(const Address& addr) { mSenderAddress = addr; }
    const Address& getSenderAddress() const { return mSenderAddress; }

    VeinsMacFrame* dup() const override { return new VeinsMacFrame(*this); }

private:
    veins::LAddress::L2Type mSenderAddress;
};

} // namespace artery

#endif /* ARTERY_VEINSMACFRAME_H_6QLH2JU5 */
