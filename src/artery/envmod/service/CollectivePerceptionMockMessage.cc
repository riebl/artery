#include "artery/envmod/service/CollectivePerceptionMockMessage.h"
#include <omnetpp.h>

namespace artery
{

Register_Class(CollectivePerceptionMockMessage)

CollectivePerceptionMockMessage::CollectivePerceptionMockMessage() :
    omnetpp::cPacket("CPM mock-up"),
    mFovContainers(std::make_shared<std::vector<FovContainer>>()),
    mObjectContainers(std::make_shared<std::vector<ObjectContainer>>())
{
}

void CollectivePerceptionMockMessage::setFovContainers(std::vector<FovContainer>&& fovs)
{
    mFovContainers = std::make_shared<std::vector<FovContainer>>(std::move(fovs));
}

void CollectivePerceptionMockMessage::setFovContainers(const std::vector<FovContainer>& fovs)
{
    mFovContainers = std::make_shared<std::vector<FovContainer>>(fovs);
}

void CollectivePerceptionMockMessage::setObjectContainers(std::vector<ObjectContainer>&& objs)
{
    mObjectContainers = std::make_shared<std::vector<ObjectContainer>>(std::move(objs));
}

void CollectivePerceptionMockMessage::setObjectContainers(const std::vector<ObjectContainer>& objs)
{
    mObjectContainers = std::make_shared<std::vector<ObjectContainer>>(objs);
}

omnetpp::cPacket* CollectivePerceptionMockMessage::dup() const
{
    return new CollectivePerceptionMockMessage(*this);
}


using namespace omnetpp;

class CpmSourceResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto cpm = dynamic_cast<CollectivePerceptionMockMessage*>(object)) {
            fire(this, t, static_cast<long>(cpm->getSourceStation()), details);
        }
    }
};

Register_ResultFilter("cpmSource", CpmSourceResultFilter)


class CpmGeneratedResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto cpm = dynamic_cast<CollectivePerceptionMockMessage*>(object)) {
            fire(this, t, cpm->getCreationTime(), details);
        }
    }
};

Register_ResultFilter("cpmGenerated", CpmGeneratedResultFilter)

} // namespace artery
