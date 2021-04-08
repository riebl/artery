#ifndef ARTERY_COLLECTIVEPERCEPTIONMOCKMESSAGE_H_YUFJ5PSV
#define ARTERY_COLLECTIVEPERCEPTIONMOCKMESSAGE_H_YUFJ5PSV

#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/envmod/sensor/FieldOfView.h"
#include "artery/envmod/sensor/SensorPosition.h"
#include <omnetpp/cpacket.h>
#include <memory>
#include <vector>

namespace artery
{

class CollectivePerceptionMockMessage : public omnetpp::cPacket
{
public:
    struct FovContainer
    {
        int sensorId = 0;
        SensorPosition position;
        FieldOfView fov;
    };

    struct ObjectContainer
    {
        omnetpp::SimTime timeOfMeasurement = omnetpp::SimTime::ZERO;
        int objectId = 0;
        int sensorId = 0;
        std::weak_ptr<EnvironmentModelObject> object;
    };

    CollectivePerceptionMockMessage();
    CollectivePerceptionMockMessage(const CollectivePerceptionMockMessage&) = default;
    CollectivePerceptionMockMessage& operator=(const CollectivePerceptionMockMessage&) = default;

    const std::vector<FovContainer>& getFovContainers() const { return *mFovContainers; }
    void setFovContainers(std::vector<FovContainer>&& fovs);
    void setFovContainers(const std::vector<FovContainer>& fovs);

    const std::vector<ObjectContainer>& getObjectContainers() const { return *mObjectContainers; }
    void setObjectContainers(std::vector<ObjectContainer>&& objs);
    void setObjectContainers(const std::vector<ObjectContainer>& objs);

    void setSourceStation(int id) { mSourceStation = id; }
    int getSourceStation() const { return mSourceStation; }

    omnetpp::cPacket* dup() const override;

private:
    int mSourceStation = 0;
    std::shared_ptr<const std::vector<FovContainer>> mFovContainers;
    std::shared_ptr<const std::vector<ObjectContainer>> mObjectContainers;
};

} // namespace artery

#endif /* ARTERY_COLLECTIVEPERCEPTIONMOCKMESSAGE_H_YUFJ5PSV */

