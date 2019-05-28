package artery.inet;

import artery.nic.IRadioDriver;

simple RadioDriver like IRadioDriver
{
	parameters:
		@class(InetRadioDriver);
		@signal[ChannelLoad](type=double);
		string macModule;
		string radioModule;

	gates:
		inout upperLayer;
		input lowerLayerIn @labels(Ieee802Ctrl/up);
		output lowerLayerOut @labels(Ieee802Ctrl/down);
		output properties;
}
