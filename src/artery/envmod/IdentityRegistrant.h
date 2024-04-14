#include "artery/utility/Identity.h"
#include <omnetpp/csimplemodule.h>


namespace artery
{

class IdentityRegistrant : public omnetpp::cSimpleModule
{
	protected:
		void initialize() override;
		void finish() override;

	private:
		artery::Identity mIdentity;

		void initializeIdentity();
		void registerIdentity();
};

} // namespace artery
