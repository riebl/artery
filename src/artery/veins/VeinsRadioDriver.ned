package artery.veins;

import artery.nic.IRadioDriver;

simple RadioDriver like IRadioDriver
{
	parameters:
		@class(VeinsRadioDriver);
		@signal[ChannelLoad](type=double);
		@statistic[ChannelLoad](record=timeavg);
		double channelLoadReportInterval = default(0.1s) @unit(s);

	gates:
		inout upperLayer;
		input lowerLayerIn;
		output lowerLayerOut;
		output properties;
}
