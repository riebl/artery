/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2018 Raphael Riebl, Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_DENSERVICE_H_
#define ARTERY_DENSERVICE_H_

#include "artery/application/den/Memory.h"
#include "artery/application/den/UseCase.h"
#include "artery/application/ItsG5BaseService.h"
#include <vanetza/asn1/denm.hpp>
#include <vanetza/btp/data_indication.hpp>
#include <vanetza/btp/data_request.hpp>
#include <cstdint>
#include <list>
#include <memory>

namespace artery
{

class Timer;

class DenService : public ItsG5BaseService
{
    public:
        DenService();
        void initialize() override;
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;
        void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>) override;
        void trigger() override;

        using ItsG5BaseService::getFacilities;
        const Timer* getTimer() const;
        std::shared_ptr<const artery::den::Memory> getMemory() const;

        ActionID_t requestActionID();
        void sendDenm(vanetza::asn1::Denm&&, vanetza::btp::DataRequestB&);

    private:
        void fillRequest(vanetza::btp::DataRequestB&);
        void initUseCases();

        const Timer* mTimer;
        uint16_t mSequenceNumber;
        std::shared_ptr<artery::den::Memory> mMemory;
        std::list<artery::den::UseCase*> mUseCases;
};

} // namespace artery

#endif
