/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_DENMOBJECT_H_
#define ARTERY_DENMOBJECT_H_

#include "artery/application/den/CauseCode.h"
#include <boost/optional.hpp>
#include <omnetpp/cobject.h>
#include <vanetza/asn1/denm.hpp>
#include <memory>

namespace artery
{

class DenmObject : public omnetpp::cObject
{
    public:
        DenmObject(vanetza::asn1::Denm&&);
        DenmObject(const vanetza::asn1::Denm&);
        DenmObject(const std::shared_ptr<const vanetza::asn1::Denm>&);
        boost::optional<den::CauseCode> situation_cause_code() const;
        const vanetza::asn1::Denm& asn1() const;

        std::shared_ptr<const vanetza::asn1::Denm> shared_ptr() const;

        omnetpp::cObject* dup() const override;

    private:
        std::shared_ptr<const vanetza::asn1::Denm> m_denm_wrapper;
};

bool operator&(const DenmObject&, den::CauseCode);

} // namespace artery

#endif /* ARTERY_DENMOBJECT_H_ */
