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
		@class(Middleware);
		@signal[IdentityUpdated](type=artery::Identity);
		@signal[IdentityRemoved](type=artery::Identity);
		@signal[StoryboardSignal](type=StoryboardSignal);

		double updateInterval = default(0.1s) @unit(s);
		string datetime;
		xml services;
		xml mcoPolicy = default(xml("<mco default=\"CCH\" />"));
}
