#ifndef ARTERY_LOCATIONTABLELOGGER_H_DPQ4NJXU
#define ARTERY_LOCATIONTABLELOGGER_H_DPQ4NJXU

#include "artery/application/ItsG5Service.h"
#include "artery/application/NetworkInterface.h"
#include "artery/application/Timer.h"
#include <vanetza/geonet/location_table.hpp>
#include <vanetza/geonet/position_vector.hpp>

namespace artery
{

class LocationTableLogger : public ItsG5Service
{
public:
    void trigger() override;
    bool requiresListener() const { return false; }

protected:
    void initialize() override;

private:
    const Timer* mTimer = nullptr;
    const vanetza::geonet::LocationTable* mLocationTable = nullptr;
    const vanetza::geonet::LongPositionVector* mEgoPositionVector = nullptr;
};

} // namespace artery

#endif /* ARTERY_LOCATIONTABLELOGGER_H_DPQ4NJXU */

