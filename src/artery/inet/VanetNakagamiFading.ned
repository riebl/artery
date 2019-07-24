package artery.inet;

import inet.physicallayer.pathloss.FreeSpacePathLoss;

//
// VanetNakagamiFading is a Nakagami-m fast fading model designed for VANETs
//
// It employs a dual-slope piecewise-linear path loss model for the average path loss envelope
// and varying shape factors for the Nakagami-m distribution depending on communication distance
//
// See the article "Mobile Vehicle-to-Vehicle Narrow-Band Channel Measurement and Characterization of the 5.9 GHz
// Dedicated Short Range Communication (DSRC) Frequency Band" by Lin Cheng et al. for details
//
module VanetNakagamiFading extends FreeSpacePathLoss
{
    parameters:
        @class(VanetNakagamiFading);
        double criticalDistance @unit(m) = default(100 m); // divides distance in first and second slope
        double gamma1 = default(2); // path loss exponent of first slope
        double gamma2 = default(4); // path loss exponent of second slope
        double sigma1 @unit(dB) = default(5.6 dB); // stdev of zero-mean normal noise in first slope
        double sigma2 @unit(dB) = default(8.1 dB); // stdev of zero-mean normal noise in second slope
        xml shapes = default(xml("<shapes default=\"0.32\"> \
                <shape distance=\"4.7\" value=\"3.1\" /> \
                <shape distance=\"11.7\" value=\"1.18\" /> \
                <shape distance=\"28.9\" value=\"1.94\" /> \
                <shape distance=\"71.6\" value=\"1.86\" /> \
                <shape distance=\"177.3\" value=\"0.45\" /> \
            </shapes>"));
}
