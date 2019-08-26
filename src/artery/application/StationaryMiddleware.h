/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_STATIONARYMIDDLEWARE_H_IE9M1YJ3
#define ARTERY_STATIONARYMIDDLEWARE_H_IE9M1YJ3

#include "artery/application/Middleware.h"

namespace artery
{

class StationaryMiddleware : public Middleware
{
    public:
        void initialize(int stage) override;
};

} // namespace artery

#endif /* ARTERY_STATIONARYMIDDLEWARE_H_IE9M1YJ3 */

