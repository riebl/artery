//
// Artery V2X Simulation Framework
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.application;

// Middleware for ITS G5 services
simple Middleware
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

		bool vanetzaEnableSecurity = default(true);
		string vanetzaCryptoBackend = default("Null");
		string vanetzaCertificateManager = default("NullOk");
		string vanetzaSecuritySignService = default("dummy");
		string vanetzaSecurityVerifyService = default("dummy");
		int vanetzaDccQueueLength = default(2);

	gates:
		input radioDriverIn;
		output radioDriverOut;
}
