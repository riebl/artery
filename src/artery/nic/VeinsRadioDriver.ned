package artery.nic;

simple VeinsRadioDriver like IRadioDriver
{
	parameters:
		@signal[ChannelLoad](type=double);
		double channelLoadReportInterval = default(0.1s) @unit(s);

	gates:
		inout middleware;
		input lowerLayerIn;
		output lowerLayerOut;
}
