/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/VanetNakagamiFading.h"
#include <boost/lexical_cast.hpp>
#include <inet/common/INETMath.h>
#include <cmath>

namespace artery
{

Define_Module(VanetNakagamiFading)

VanetNakagamiFading::VanetNakagamiFading() :
    m_critical_distance(inet::m(100.0)),
    m_gamma1(2), m_gamma2(4),
    m_sigma1(1), m_sigma2(1),
    m_default_shape(1)
{
}

void VanetNakagamiFading::initialize(int stage)
{
    if (stage == inet::INITSTAGE_LOCAL) {
        m_critical_distance = inet::m(par("criticalDistance"));
        if (m_critical_distance < inet::m(1)) {
            throw omnetpp::cRuntimeError("criticalDistance is expected to be greater than 1 meter");
        }
        m_gamma1 = par("gamma1");
        m_gamma2 = par("gamma2");
        m_sigma1 = par("sigma1");
        m_sigma2 = par("sigma2");
        parseShapeFactors(par("shapes"));
    }
}

void VanetNakagamiFading::parseShapeFactors(const omnetpp::cXMLElement* shapes)
{
    m_shapes.clear();

    if (shapes && strcmp(shapes->getTagName(), "shapes") == 0) {
        const char* default_value = shapes->getAttribute("default");
        if (default_value) {
            m_default_shape = boost::lexical_cast<decltype(m_default_shape)>(default_value);
        } else {
            throw omnetpp::cRuntimeError("Missing mandatory 'default' attribute in root <shapes> tag");
        }

        for (const omnetpp::cXMLElement* shape : shapes->getChildrenByTagName("shape"))
        {
            const char* dist_attr = shape->getAttribute("distance");
            if (!dist_attr) {
                throw omnetpp::cRuntimeError("XML Nakagami configuration contains <shape> tag without 'distance' attribute");
            }

            const char* value_attr = shape->getAttribute("value");
            if (!value_attr) {
                throw omnetpp::cRuntimeError("XML Nakagami configuration contains <shape> tag without 'value' attribute");
            }

            auto dist = inet::m { boost::lexical_cast<double>(dist_attr) };
            auto value = boost::lexical_cast<double>(value_attr);
            m_shapes[dist] = value;
        }
    } else {
        throw omnetpp::cRuntimeError("XML Nakagami shape factor configuration does not start with <shapes> tag");
    }
}

double VanetNakagamiFading::computePathLoss(inet::mps propagationSpeed, inet::Hz freq, inet::m dist) const
{
    inet::m waveLength = propagationSpeed / freq;
    return computeNakagamiPathLoss(waveLength, dist);
}

double VanetNakagamiFading::computeNakagamiPathLoss(inet::m lambda, inet::m dist) const
{
    const double shapeFactor = lookUpShapeFactor(dist);
    const double omega = computeDualSlopePathLoss(lambda, dist);
    return gamma_d(shapeFactor, omega / shapeFactor);
}

double VanetNakagamiFading::computeDualSlopePathLoss(inet::m lambda, inet::m dist) const
{
    static const inet::m refDist = inet::m(1.0);
    const double refLoss = computeFreeSpacePathLoss(lambda, refDist, alpha, systemLoss);

    double loss = 0.0;
    if (dist < refDist) {
        loss = refLoss;
    } else if (dist < m_critical_distance) {
        loss = 10.0 * m_gamma1 * std::log10(inet::unit(dist / refDist).get()) + normal(0.0, m_sigma1);
    } else {
        loss = 10.0 * m_gamma1 * std::log10(inet::unit(m_critical_distance / refDist).get())
            + 10.0 * m_gamma2 * std::log10(inet::unit(dist / m_critical_distance).get()) + normal(0.0, m_sigma2);
    }

    return refLoss / inet::math::dB2fraction(loss);
}

double VanetNakagamiFading::lookUpShapeFactor(inet::m dist) const
{
    double shapeFactor = m_default_shape;
    auto found = m_shapes.lower_bound(dist);
    if (found != m_shapes.end()) {
        shapeFactor = found->second;
    }
    EV_DEBUG << "Nakagami m = " << shapeFactor << " for distance " << dist << "\n";
    return shapeFactor;
}

std::ostream& VanetNakagamiFading::printToStream(std::ostream& os, int level) const
{
    os << "VanetNakagamiFading";
    if (level <= PRINT_LEVEL_TRACE) {
        os << ", alpha = " << alpha
            << ", systemLoss = " << systemLoss
            << ", criticalDistance = " << m_critical_distance
            << ", gamma1 = " << m_gamma1
            << ", gamma2 = " << m_gamma2
            << ", sigma1 = " << m_sigma1
            << ", sigma2 = " << m_sigma2
            << ", default shape factor = " << m_default_shape
            << ", " << m_shapes.size() << "shape factors";
    }
    return os;
}

} // namespace artery
