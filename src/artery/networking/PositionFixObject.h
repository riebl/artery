#ifndef ARTERY_POSITIONFIXOBJECT_H_UOLSCGFJ
#define ARTERY_POSITIONFIXOBJECT_H_UOLSCGFJ

#include <omnetpp/cobject.h>
#include <vanetza/common/position_fix.hpp>

namespace artery
{

class PositionFixObject : public omnetpp::cObject, public vanetza::PositionFix
{
};

} // namespace artery

#endif /* ARTERY_POSITIONFIXOBJECT_H_UOLSCGFJ */

