package artery.testbed;

simple OtaInterfaceLayer
{
	parameters:
		@class(OtaInterfaceLayer);
		string otaInterfaceModule = default("otaInterface");
		string radioDriverModule;
		string mobilityModule = default(".^.mobility");

	gates:
		input lowerLayerIn;
		output lowerLayerOut;
		input radioDriverProperties @loose;
}
