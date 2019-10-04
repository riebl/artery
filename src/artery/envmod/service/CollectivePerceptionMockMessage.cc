#include "artery/envmod/service/CollectivePerceptionMockMessage.h"

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

} // namespace artery
