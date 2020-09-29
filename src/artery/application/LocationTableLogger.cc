#include "artery/application/LocationTableLogger.h"
#include "artery/networking/Router.h"
#include <boost/optional/optional.hpp>
#include <inet/common/ModuleAccess.h>

namespace artery
{

static const omnetpp::simsignal_t scCommunicationRangeSignal = omnetpp::cComponent::registerSignal("CommunicationRange");
static const omnetpp::simsignal_t scNeighbourCountSignal = omnetpp::cComponent::registerSignal("NeighbourCount");
static const omnetpp::simsignal_t scNeighbourDensitySignal = omnetpp::cComponent::registerSignal("NeighbourDensity");

Define_Module(LocationTableLogger)

void LocationTableLogger::initialize()
{
    ItsG5BaseService::initialize();
    auto router = inet::getModuleFromPar<Router>(par("routerModule"), findHost());
    mLocationTable = &router->getLocationTable();
    mEgoPositionVector = &router->getEgoPositionVector();
    mTimer = &getFacilities().get_const<Timer>();
}

void LocationTableLogger::trigger()
{
    namespace gn = vanetza::geonet;

    struct LocationTableVisitor
    {
        LocationTableVisitor(gn::GeodeticPosition ego, gn::Timestamp deadline) :
            egoPosition(ego), pvDeadline(deadline)
        {
        }

        LocationTableVisitor(const LocationTableVisitor&) = delete;
        LocationTableVisitor& operator=(const LocationTableVisitor&) = delete;

        void operator()(const vanetza::MacAddress&, const gn::LocationTableEntry& locte)
        {
            if (locte.has_position_vector()) {
                const vanetza::geonet::LongPositionVector& pv = locte.get_position_vector();
                auto range = gn::distance(pv.position(), egoPosition) / vanetza::units::si::meter;
                if (pv.timestamp >= pvDeadline) {
                    if (range > communicationRange) {
                        communicationRange = range;
                    }
                    ++neighbourDensity;
                }
            }

            if (locte.is_neighbour()) {
                ++neighbourCount;
            }
        }

        const gn::GeodeticPosition egoPosition;
        const gn::Timestamp pvDeadline;
        double communicationRange = 0.0;
        unsigned neighbourDensity = 0;
        unsigned neighbourCount = 0;
    };

    gn::Timestamp deadline { mTimer->getCurrentTime() - std::chrono::seconds(1) };
    LocationTableVisitor visitor(mEgoPositionVector->position(), deadline);
    mLocationTable->visit(std::ref(visitor));

    emit(scCommunicationRangeSignal, visitor.communicationRange);
    emit(scNeighbourDensitySignal, visitor.neighbourDensity);
    emit(scNeighbourCountSignal, visitor.neighbourCount);
}

} // namespace artery
