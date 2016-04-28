#include <gtest/gtest.h>
#include <vanetza/geonet/data_request.hpp>
#include <vanetza/units/time.hpp>

using namespace vanetza::geonet;
using vanetza::units::si::seconds;

TEST(DataRequest, repetition) {
    MIB mib;
    DataRequest r(mib);
    EXPECT_FALSE(!!r.repetition);

    r.repetition = DataRequest::Repetition();
    EXPECT_TRUE(!!r.repetition);
}

TEST(DataRequest, has_further_repetition) {
    DataRequest::Repetition repetition;
    repetition.interval = 10.0 * seconds;
    repetition.maximum = 0.0 * seconds;
    EXPECT_FALSE(has_further_repetition(repetition));

    repetition.interval = 0.0 * seconds;
    EXPECT_FALSE(has_further_repetition(repetition));

    repetition.maximum = -10.0 * seconds;
    repetition.interval = -15.0 * seconds;
    EXPECT_FALSE(has_further_repetition(repetition));

    repetition.maximum = 30.0 * seconds;
    repetition.interval = 10.0 * seconds;
    EXPECT_TRUE(has_further_repetition(repetition));

    repetition.interval = 0.0 * seconds;
    EXPECT_FALSE(has_further_repetition(repetition));
}

TEST(DataRequest, decrement_by_one) {
    DataRequest::Repetition repetition;
    repetition.maximum = 30.0 * seconds;
    repetition.interval = 10.0 * seconds;
    decrement_by_one(repetition);
    EXPECT_DOUBLE_EQ(repetition.interval / seconds, 10.0);
    EXPECT_DOUBLE_EQ(repetition.maximum / seconds, 20.0);

    repetition.interval = 5.0 * seconds;
    decrement_by_one(repetition);
    EXPECT_DOUBLE_EQ(repetition.maximum / seconds, 15.0);

    repetition.interval = 20.0 * seconds;
    decrement_by_one(repetition);
    EXPECT_DOUBLE_EQ(repetition.maximum / seconds, 0.0);
    EXPECT_DOUBLE_EQ(repetition.interval / seconds, 20.0);

    repetition.maximum = -30.0 * seconds;
    repetition.interval = -60.0 * seconds;
    decrement_by_one(repetition);
    EXPECT_DOUBLE_EQ(repetition.maximum / seconds, 0.0);

    repetition.maximum = -30.0 * seconds;
    repetition.interval = -10.0 * seconds;
    decrement_by_one(repetition);
    EXPECT_DOUBLE_EQ(repetition.maximum / seconds, 0.0);

    repetition.maximum = 30.0 * seconds;
    repetition.interval = -10.0 * seconds;
    decrement_by_one(repetition);
    EXPECT_DOUBLE_EQ(repetition.maximum / seconds, 0.0);
}
