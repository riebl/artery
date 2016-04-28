/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_ENVMOD_ENVMODPRINTER_H_
#define ARTERY_ENVMOD_ENVMODPRINTER_H_

#include "artery/application/ItsG5Service.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include <string>

namespace artery
{

/**
 * Prints currently visible objects (i.e. vehicles) as seen via Radar or CA messages.
 * This service is meant for debugging and demonstration purposes.
 */
class EnvmodPrinter : public ItsG5Service
{
    public:
        void initialize() override;
        void trigger() override;

    private:
        void printSensorObjectList(const std::string& title, const TrackedObjectsFilterRange&);

        LocalEnvironmentModel* mLocalEnvironmentModel;
        std::string mEgoId;
};

} // namespace artery

#endif /* ARTERY_ENVMOD_ENVMODPRINTER_H_ */
