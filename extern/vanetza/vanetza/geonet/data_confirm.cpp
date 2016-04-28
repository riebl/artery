#include "data_confirm.hpp"
#include "data_request.hpp"

namespace vanetza
{
namespace geonet
{

DataConfirm& operator ^=(DataConfirm& lhs, DataConfirm::ResultCode rhs)
{
    if (rhs != DataConfirm::ResultCode::ACCEPTED) {
        lhs.result_code = rhs;
    }
    return lhs;
}

DataConfirm::ResultCode validate_data_request(const DataRequest& req, const MIB& mib)
{
    DataConfirm::ResultCode result = DataConfirm::ResultCode::REJECTED_UNSPECIFIED;

    // TODO: traffic class validation
    if (req.maximum_lifetime > mib.itsGnMaxPacketLifetime) {
        result = DataConfirm::ResultCode::REJECTED_MAX_LIFETIME;
    } else if (req.repetition && req.repetition->interval < mib.itsGnMinPacketRepetitionInterval) {
        result = DataConfirm::ResultCode::REJECTED_MIN_REPETITION_INTERVAL;
    } else {
        result = DataConfirm::ResultCode::ACCEPTED;
    }

    return result;
}

DataConfirm::ResultCode validate_data_request(const DataRequestWithArea& req, const MIB& mib)
{
    if (area_size(req.destination) > mib.itsGnMaxGeoAreaSize) {
        return DataConfirm::ResultCode::REJECTED_MAX_GEO_AREA_SIZE;
    } else {
        return validate_data_request(static_cast<const DataRequest&>(req), mib);
    }
}

DataConfirm::ResultCode validate_payload(const std::unique_ptr<DownPacket>& payload, const MIB& mib)
{
    DataConfirm::ResultCode result = DataConfirm::ResultCode::REJECTED_UNSPECIFIED;

    if (!payload) {
        // leave code to unspecified
    } else if (payload->size() > mib.itsGnMaxSduSize) {
        result = DataConfirm::ResultCode::REJECTED_MAX_SDU_SIZE;
    } else {
        result = DataConfirm::ResultCode::ACCEPTED;
    }

    return result;
}

} // namespace geonet
} // namespace vanetza

