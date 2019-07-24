/*
 * Artery V2X Simulation Framework
 * Copyright 2019 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_VANETNAKAGAMIFADING_H_KRHFTXO9
#define ARTERY_VANETNAKAGAMIFADING_H_KRHFTXO9

#include <inet/physicallayer/pathloss/FreeSpacePathLoss.h>
#include <map>

namespace artery
{

class VanetNakagamiFading : public inet::physicallayer::FreeSpacePathLoss
{
public:
    VanetNakagamiFading();

    double computePathLoss(inet::mps propagationSpeed, inet::Hz freq, inet::m dist) const override;
    std::ostream& printToStream(std::ostream&, int level) const override;

protected:
    void initialize(int stage) override;
    void parseShapeFactors(const omnetpp::cXMLElement*);
    double lookUpShapeFactor(inet::m dist) const;
    double computeDualSlopePathLoss(inet::m waveLength, inet::m dist) const;
    double computeNakagamiPathLoss(inet::m waveLength, inet::m dist) const;

private:
    inet::m m_critical_distance;
    double m_gamma1; /*< path loss exponent below critical distance */
    double m_gamma2; /*< path loss exponent beyond critical distance */
    double m_sigma1; /*< stdev below critical distance */
    double m_sigma2; /*< stdev beyond critical distance */
    double m_default_shape; /*< default Nakagami-m shape factor for distance not covered by map */
    std::map<inet::m, double> m_shapes; /*< Nakagami-m shape factors depending on distance */
};

} // namespace artery

#endif /* ARTERY_VANETNAKAGAMIFADING_H_KRHFTXO9 */

