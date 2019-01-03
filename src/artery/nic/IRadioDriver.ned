package artery.nic;

moduleinterface IRadioDriver
{
	parameters:
		@signal[ChannelLoad](type=double);

	gates:
		inout upperLayer;
		output properties;
}
