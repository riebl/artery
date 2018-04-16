#ifndef ARTERY_OTA_INDICATION_QUEUE_H
#define ARTERY_OTA_INDICATION_QUEUE_H

#include "artery/messages/GeoNetPacket_m.h"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <vector>

namespace artery
{

class OtaInterface;

class OtaIndicationQueue
{
public:
    OtaIndicationQueue(OtaInterface* interface);

    virtual void waitFor(std::chrono::microseconds);
    virtual void trigger(std::unique_ptr<GeoNetPacket>);
    virtual void flushQueue();

private:
    std::condition_variable mCondVar;
    std::vector<std::unique_ptr<GeoNetPacket>> mIndicationList;
    std::mutex mMutex;
    OtaInterface* mOtaInterface;
    bool mNotified;
};

} // namespace artery

#endif /* ARTERY_OTA_INDICATION_QUEUE_H */

