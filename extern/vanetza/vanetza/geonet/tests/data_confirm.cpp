#include <gtest/gtest.h>
#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/geonet/data_confirm.hpp>
#include <vanetza/geonet/data_request.hpp>
#include <vanetza/geonet/packet.hpp>
#include <algorithm>

using namespace vanetza::geonet;
using vanetza::units::si::seconds;
using vanetza::units::si::meter;

TEST(DataConfirm, ctor) {
    DataConfirm a;
    EXPECT_EQ(a.result_code, DataConfirm::ResultCode::ACCEPTED);
    DataConfirm b(DataConfirm::ResultCode::REJECTED_UNSPECIFIED);
    EXPECT_EQ(b.result_code, DataConfirm::ResultCode::REJECTED_UNSPECIFIED);
}

TEST(DataConfirm, accepted_rejected) {
    DataConfirm a(DataConfirm::ResultCode::REJECTED_MAX_LIFETIME);
    EXPECT_TRUE(a.rejected());
    EXPECT_FALSE(a.accepted());
    a.result_code = DataConfirm::ResultCode::ACCEPTED;
    EXPECT_FALSE(a.rejected());
    EXPECT_TRUE(a.accepted());
}

TEST(DataConfirm, validate_data_request) {
    MIB mib;
    DataRequest req(mib);
    EXPECT_EQ(validate_data_request(req, mib),
            DataConfirm::ResultCode::ACCEPTED);

    DataRequest req_lt(req);
    req_lt.maximum_lifetime.encode(mib.itsGnMaxPacketLifetime.decode() + 10.0 * seconds);
    EXPECT_EQ(validate_data_request(req_lt, mib),
            DataConfirm::ResultCode::REJECTED_MAX_LIFETIME);

    DataRequest req_rep(req);
    req_rep.repetition = DataRequest::Repetition();
    req_rep.repetition->interval = mib.itsGnMinPacketRepetitionInterval - 1 * seconds;
    EXPECT_EQ(validate_data_request(req_rep, mib),
            DataConfirm::ResultCode::REJECTED_MIN_REPETITION_INTERVAL);
}

TEST(DataConfirm, validate_data_request_with_area) {
    MIB mib;
    DataRequestWithArea req(mib);
    EXPECT_EQ(validate_data_request(req, mib),
            DataConfirm::ResultCode::ACCEPTED);

    Circle c;
    // radius = magnitude of max area size -> circle area is much larger
    c.r = vanetza::units::Length(mib.itsGnMaxGeoAreaSize / meter); // hack!
    req.destination.shape = c;
    EXPECT_EQ(validate_data_request(req, mib),
            DataConfirm::ResultCode::REJECTED_MAX_GEO_AREA_SIZE);
}

TEST(DataConfirm, validate_payload) {
    MIB mib;
    std::unique_ptr<DownPacket> no_payload;
    std::unique_ptr<DownPacket> giant_payload(new DownPacket());
    {
        vanetza::ByteBuffer giant_buffer;
        std::fill_n(std::back_inserter(giant_buffer), 2048, 0x0f);
        (*giant_payload)[vanetza::OsiLayer::Link] = std::move(giant_buffer);
    }
    std::unique_ptr<DownPacket> ok_payload(new DownPacket());

    EXPECT_EQ(validate_payload(no_payload, mib),
            DataConfirm::ResultCode::REJECTED_UNSPECIFIED);
    EXPECT_EQ(validate_payload(giant_payload, mib),
            DataConfirm::ResultCode::REJECTED_MAX_SDU_SIZE);
    EXPECT_EQ(validate_payload(ok_payload, mib),
            DataConfirm::ResultCode::ACCEPTED);
}

TEST(DataConfirm, xor_op) {
    DataConfirm a;
    EXPECT_EQ(a.result_code, DataConfirm::ResultCode::ACCEPTED);
    a ^= DataConfirm::ResultCode::REJECTED_MAX_LIFETIME;
    EXPECT_EQ(a.result_code, DataConfirm::ResultCode::REJECTED_MAX_LIFETIME);
    a ^= DataConfirm::ResultCode::ACCEPTED;
    EXPECT_EQ(a.result_code, DataConfirm::ResultCode::REJECTED_MAX_LIFETIME);
    a ^= DataConfirm::ResultCode::REJECTED_UNSPECIFIED;
    EXPECT_EQ(a.result_code, DataConfirm::ResultCode::REJECTED_UNSPECIFIED);
}

