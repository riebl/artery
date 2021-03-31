/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2021 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_PERSONMIDDLEWARE_H_IH5GPCMP
#define ARTERY_PERSONMIDDLEWARE_H_IH5GPCMP

#include "artery/application/Middleware.h"
#include "artery/traci/PersonMobility.h"

namespace artery
{

class PersonMiddleware : public Middleware
{
    public:
        void initialize(int stage) override;

    private:
        PersonMobility* mMobility = nullptr;
};

} // namespace artery

#endif /* ARTERY_PERSONMIDDLEWARE_H_IH5GPCMP */

