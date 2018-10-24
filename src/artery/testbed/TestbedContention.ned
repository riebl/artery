package artery.testbed;

import inet.linklayer.ieee80211.mac.contention.Contention;

simple TestbedContention extends Contention
{
	parameters:
		@class(TestbedContention);
		// IFS defaults to AC_VO AIFS (highest priority)
		double interFrameSpacing = default(0.000058) @unit(s);
}
