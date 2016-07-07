#ifndef ARTERY_INET_TRACIMOBILITY_H_YUPKWFWC
#define ARTERY_INET_TRACIMOBILITY_H_YUPKWFWC

#include "inet/mobility/contract/IMobility.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"

class InetTraCIMobility : public Veins::TraCIMobility, public inet::IMobility
{
public:
    // from inet::IMobility
    double getMaxSpeed() const  override;
    inet::Coord getCurrentPosition() override;
    inet::Coord getCurrentSpeed() override;
    inet::EulerAngles getCurrentAngularPosition() override;
    inet::EulerAngles getCurrentAngularSpeed() override;
    inet::Coord getConstraintAreaMax() const override;
    inet::Coord getConstraintAreaMin() const override;

    // from Veins::TraCIMobility
    Veins::TraCIScenarioManagerBase* getManager() const override;

protected:
    // from BaseMobility (Veins)
    void updatePosition() override;

private:
    inet::Coord convert(const Coord&) const;

    typedef Veins::TraCIMobility veins_base;
    typedef inet::IMobility inet_base;
};

#endif /* ARTERY_INET_TRACIMOBILITY_H_YUPKWFWC */

