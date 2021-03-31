//
// Artery V2X Simulation Framework
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.application;

// Middleware for ITS G5 services
simple PersonMiddleware extends Middleware
{
	parameters:
		@class(PersonMiddleware);
		@signal[IdentityChanged](type=long);

		string localEnvironmentModule = default("");
		string globalEnvironmentModule = default("");
		string mobilityModule;
}
