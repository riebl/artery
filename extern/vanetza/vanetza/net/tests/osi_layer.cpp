#include <gtest/gtest.h>
#include <vanetza/net/osi_layer.hpp>

using namespace vanetza;

TEST(OsiLayer, ordering) {
    EXPECT_LT(OsiLayer::Physical, OsiLayer::Link);
    EXPECT_LT(OsiLayer::Link, OsiLayer::Network);
    EXPECT_LT(OsiLayer::Network, OsiLayer::Transport);
    EXPECT_LT(OsiLayer::Transport, OsiLayer::Session);
    EXPECT_LT(OsiLayer::Session, OsiLayer::Presentation);
    EXPECT_LT(OsiLayer::Presentation, OsiLayer::Application);

    EXPECT_EQ(min_osi_layer(), OsiLayer::Physical);
    EXPECT_EQ(max_osi_layer(), OsiLayer::Application);
}

TEST(OsiLayer, comparison) {
    EXPECT_LT(OsiLayer::Physical, OsiLayer::Link);
    EXPECT_GT(OsiLayer::Link, OsiLayer::Physical);
    EXPECT_EQ(OsiLayer::Physical, OsiLayer::Physical);
    EXPECT_NE(OsiLayer::Physical, OsiLayer::Link);
    EXPECT_LE(OsiLayer::Physical, OsiLayer::Physical);
    EXPECT_GE(OsiLayer::Link, OsiLayer::Link);
}

TEST(OsiLayer, list) {
    auto list = osi_layers;
    ASSERT_EQ(list.size(), 7);

    auto it = list.begin();
    EXPECT_EQ(OsiLayer::Physical, *it++);
    EXPECT_EQ(OsiLayer::Link, *it++);
    EXPECT_EQ(OsiLayer::Network, *it++);
    EXPECT_EQ(OsiLayer::Transport, *it++);
    EXPECT_EQ(OsiLayer::Session, *it++);
    EXPECT_EQ(OsiLayer::Presentation, *it++);
    EXPECT_EQ(OsiLayer::Application, *it++);
    EXPECT_EQ(list.end(), it);

    auto prev = list.begin();
    for (auto it = prev + 1; it != list.end(); prev = it, ++it) {
        EXPECT_LT(*prev, *it);
    }
}

TEST(OsiLayer, num) {
    EXPECT_EQ(7, num_osi_layers(min_osi_layer(), max_osi_layer()));
    EXPECT_EQ(1, num_osi_layers(OsiLayer::Link, OsiLayer::Link));
    EXPECT_EQ(2, num_osi_layers(OsiLayer::Link, OsiLayer::Network));
    EXPECT_EQ(0, num_osi_layers(OsiLayer::Network, OsiLayer::Link));
}

TEST(OsiLayer, distance) {
    EXPECT_EQ(0, distance(OsiLayer::Session, OsiLayer::Session));
    EXPECT_EQ(1, distance(OsiLayer::Network, OsiLayer::Transport));
    EXPECT_EQ(-1, distance(OsiLayer::Transport, OsiLayer::Network));
    EXPECT_EQ(num_osi_layers(min_osi_layer(), max_osi_layer()) - 1,
        distance(min_osi_layer(), max_osi_layer()));
}

TEST(OsiLayer, compile_time_range) {
    const auto r1_ref = osi_layers;
    ASSERT_EQ(7, r1_ref.size());
    auto r1 = osi_layer_range<min_osi_layer(), max_osi_layer()>();
    EXPECT_EQ(r1_ref.size(), r1.size());
    for (unsigned i = 0; i < r1.size(); ++i) {
        EXPECT_EQ(r1_ref[i], r1[i]);
    }

    auto r2 = osi_layer_range<OsiLayer::Link, OsiLayer::Transport>();
    ASSERT_EQ(3, r2.size());
    EXPECT_EQ(OsiLayer::Link, r2[0]);
    EXPECT_EQ(OsiLayer::Network, r2[1]);
    EXPECT_EQ(OsiLayer::Transport, r2[2]);

    auto r3 = osi_layer_range<OsiLayer::Application, OsiLayer::Application>();
    EXPECT_EQ(1, r3.size());
}

TEST(OsiLayer, run_time_range) {
    const auto r1_ref = osi_layers;
    ASSERT_EQ(7, r1_ref.size());
    auto r1 = osi_layer_range(min_osi_layer(), max_osi_layer());
    EXPECT_EQ(r1_ref.size(), r1.size());
    for (unsigned i = 0; i < r1.size(); ++i) {
        EXPECT_EQ(r1_ref[i], r1[i]);
    }

    auto r2 = osi_layer_range(OsiLayer::Link, OsiLayer::Transport);
    ASSERT_EQ(3, r2.size());
    EXPECT_EQ(OsiLayer::Link, r2[0]);
    EXPECT_EQ(OsiLayer::Network, r2[1]);
    EXPECT_EQ(OsiLayer::Transport, r2[2]);

    auto r3 = osi_layer_range(OsiLayer::Application, OsiLayer::Application);
    ASSERT_EQ(1, r3.size());
    EXPECT_EQ(OsiLayer::Application, r3[0]);
}
