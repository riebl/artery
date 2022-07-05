/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_CPOBJECT_H_
#define ARTERY_CPOBJECT_H_

/**
 * @file CpObject.h
 * @brief Collective Perception Message wrapper object
 */

#include <omnetpp/cobject.h>
#include <vanetza/asn1/cpm.hpp>

#include <memory>


namespace artery
{

/**
 * @brief Collective Perception Message wrapper object
 *
 * Stores and owns a CPM so that it can be passed to other modules using OMNeT++ infrastructure.
 */
class CpObject : public omnetpp::cObject
{
public:
    CpObject(const CpObject&) = default;
    CpObject& operator=(const CpObject&) = default;

    CpObject(vanetza::asn1::Cpm&&);
    CpObject& operator=(vanetza::asn1::Cpm&&);

    CpObject(const vanetza::asn1::Cpm&);
    CpObject& operator=(const vanetza::asn1::Cpm&);

    CpObject(std::shared_ptr<const vanetza::asn1::Cpm>);
    CpObject& operator=(std::shared_ptr<const vanetza::asn1::Cpm>);

    /**
     * @brief Returns a reference of the stored CPM
     *
     * @return Reference of the stored CPM
     */
    const vanetza::asn1::Cpm& asn1() const;

    /**
     * @brief Returns a shared_ptr of the stored CPM
     *
     * @return shared_ptr of the stored CPM
     */
    std::shared_ptr<const vanetza::asn1::Cpm> shared_ptr();

    omnetpp::cObject* dup() const override;

private:
    /**
     * @brief The stored CPM
     */
    std::shared_ptr<const vanetza::asn1::Cpm> m_wrapper;
};

}  // namespace artery

#endif /* ARTERY_CPOBJECT_H_ */
