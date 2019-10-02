#ifndef ARTERY_POSITIONPROVIDER_H_7KXLZPSI
#define ARTERY_POSITIONPROVIDER_H_7KXLZPSI

#include "artery/utility/Geometry.h"

namespace artery
{

class PositionProvider
{
    public:
        virtual Position getCartesianPosition() const = 0;
        virtual GeoPosition getGeodeticPosition() const = 0;
};

} // namespace artery

#endif /* ARTERY_POSITIONPROVIDER_H_7KXLZPSI */

