package artery.inet;

import inet.mobility.contract.IMobility;
import org.car2x.veins.modules.mobility.traci.TraCIMobility;

simple InetTraCIMobility extends TraCIMobility like IMobility
{
	parameters:
		@class(InetTraCIMobility);
		@signal[mobilityStateChanged];
}
