package artery.nic;

simple VeinsRadioDriver like IRadioDriver
{
	parameters:
		@class(artery::VeinsRadioDriver);
		@signal[ChannelLoad](type=double);
		double channelLoadReportInterval = default(0.1s) @unit(s);

	gates:
		inout upperLayer;
		input lowerLayerIn;
		output lowerLayerOut;
}
