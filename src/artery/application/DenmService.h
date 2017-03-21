/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_DENMSERVICE_H_
#define ARTERY_DENMSERVICE_H_

#include "artery/application/DenmMemory.h"
#include "artery/application/DenmUseCase.h"
#include "artery/application/ItsG5BaseService.h"
#include <boost/ptr_container/ptr_list.hpp>
#include <vanetza/asn1/denm.hpp>
#include <vanetza/btp/data_indication.hpp>
#include <cstdint>
#include <list>
#include <memory>

class Timer;

class DenmService : public ItsG5BaseService
{
    public:
        DenmService();
        void initialize() override;
        void receiveSignal(cComponent*, omnetpp::simsignal_t, cObject*, cObject*) override;
        void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>) override;
        void trigger() override;

    private:
        void sendDenm(artery::denm::UseCase&);
        vanetza::btp::DataRequestB createRequest(artery::denm::UseCase&);
        vanetza::asn1::Denm createDenm(artery::denm::UseCase&);

        const Timer* mTimer;
        uint16_t mSequenceNumber;
        std::unique_ptr<artery::denm::Memory> mDenmMemory;
        boost::ptr_list<artery::denm::UseCase> mUseCases;
};

#endif
