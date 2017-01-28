package artery.nic;

simple VeinsRadioDriver like IRadioDriver
{
	parameters:
		@signal[ChannelLoad](type=double);

	gates:
		inout middleware;
		input lowerLayerIn;
		output lowerLayerOut;
		input lowerControlIn;
		output lowerControlOut;
}
