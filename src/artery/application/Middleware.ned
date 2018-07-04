//
// Artery V2X Simulation Framework
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.application;

moduleinterface IMiddleware
{
}

// Middleware for ITS G5 services
simple Middleware like IMiddleware
{
	parameters:
		@class(artery::Middleware);
		@signal[Identity.update](type=artery::Identity);
		@signal[Identity.remove](type=artery::Identity);
		@signal[StoryboardSignal](type=StoryboardSignal);

		string radioDriverModule;
		double updateInterval = default(0.1s) @unit(s);
		string datetime;
		xml services;

		bool vanetzaDeferInitialBeacon = default(true);
		bool vanetzaEnableSecurity = default(true);
		string vanetzaCryptoBackend = default("Null");
		string vanetzaCertificateProvider = default("Null");
		string vanetzaCertificateValidator = default("NullOk");
		string vanetzaSecuritySignService = default("dummy");
		string vanetzaSecurityVerifyService = default("dummy");
		int vanetzaDccQueueLength = default(2);

	gates:
		input radioDriverIn;
		output radioDriverOut;
}
