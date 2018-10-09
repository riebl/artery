package artery.inet;

import artery.nic.IRadioDriver;

simple InetRadioDriver like IRadioDriver
{
	parameters:
		@class(artery::InetRadioDriver);
		@signal[ChannelLoad](type=double);
		string macModule;

	gates:
		inout upperLayer;
		input lowerLayerIn @labels(Ieee802Ctrl/up);
		output lowerLayerOut @labels(Ieee802Ctrl/down);
}
