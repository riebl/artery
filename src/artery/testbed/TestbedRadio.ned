package artery.testbed;

import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadio;

module TestbedRadio extends Ieee80211ScalarRadio
{
	parameters:
		@class(TestbedRadio);
		double reachableTime = default(0.5s) @unit(s);
}
