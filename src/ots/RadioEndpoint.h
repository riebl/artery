#ifndef OTS_RADIOENDPOINT_H_MXQA3MA3
#define OTS_RADIOENDPOINT_H_MXQA3MA3

#include <memory>

namespace ots
{

// forward declaration
class RadioMessage;

/**
 * Interface for radio interaction with OTS
 */
class RadioEndpoint
{
public:
    virtual ~RadioEndpoint() = default;

    /**
     * Notify about transmission request by OTS
     * \param msg message to be transmitted
     */
    virtual void onRadioTransmit(std::unique_ptr<RadioMessage> msg) = 0;
};

} // namespace ots

#endif /* OTS_RADIOENDPOINT_H_MXQA3MA3 */