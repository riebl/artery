#ifndef ANTENNAMOBILITY_H_BMWHZWNJ
#define ANTENNAMOBILITY_H_BMWHZWNJ

#include <inet/mobility/contract/IMobility.h>
#include <inet/common/geometry/common/Rotation.h>

class AntennaMobility : public inet::IMobility, public omnetpp::cSimpleModule
{
public:
    // inet::IMobility interface
    double getMaxSpeed() const override;
    inet::Coord getCurrentPosition() override;
    inet::Coord getCurrentSpeed() override;
    inet::EulerAngles getCurrentAngularPosition() override;
    inet::EulerAngles getCurrentAngularSpeed() override;
    inet::Coord getConstraintAreaMax() const override;
    inet::Coord getConstraintAreaMin() const override;

    // omnetpp::cSimpleModule
    void initialize(int stage) override;
    int numInitStages() const override;

private:
    inet::IMobility* mParentMobility = nullptr;
    inet::Coord mOffsetCoord;
    inet::EulerAngles mOffsetAngles;
    inet::Rotation mOffsetRotation;
};

#endif /* ANTENNAMOBILITY_H_BMWHZWNJ */
