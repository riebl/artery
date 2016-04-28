#include <gtest/gtest.h>
#include <vanetza/geonet/address.hpp>
#include <vanetza/geonet/location_table.hpp>
#include <vanetza/geonet/mib.hpp>
#include <vanetza/geonet/position_vector.hpp>
#include <algorithm>

using namespace vanetza::geonet;
const MIB cMIB;
const Timestamp::absolute_unit_type abs_ms;

TEST(LocationTable, has_entry) {
    LocationTable lt(cMIB);
    Address a;
    a.mid({1, 2, 3, 4, 5, 6});
    EXPECT_FALSE(lt.has_entry(a));

    LongPositionVector pv;
    pv.gn_addr = a;
    lt.update(pv);
    EXPECT_TRUE(lt.has_entry(a));

    // only MID of GN_ADDR should be used for look-up
    a.country_code(42);
    EXPECT_TRUE(lt.has_entry(a));

    a.mid({0, 2, 3, 4, 5, 6});
    EXPECT_FALSE(lt.has_entry(a));
}

TEST(LocationTable, position_vector) {
    LocationTable lt(cMIB);
    Address a;
    a.mid({1, 2, 3, 4, 5, 6});
    EXPECT_FALSE(lt.get_position(a));

    LongPositionVector pv;
    pv.gn_addr = a;
    lt.update(pv);

    auto retrieved_pv = lt.get_position(a);
    ASSERT_TRUE(!!retrieved_pv);
    EXPECT_EQ(pv, retrieved_pv.get());
}

TEST(LocationTable, neighbourhood) {
    LocationTable lt(cMIB);
    EXPECT_FALSE(lt.has_neighbours());

    Address addr_a;
    addr_a.mid({1, 2, 3, 4, 5 ,6});
    LongPositionVector pv_a;
    pv_a.gn_addr = addr_a;
    lt.update(pv_a);
    EXPECT_FALSE(lt.has_neighbours());

    lt.is_neighbour(addr_a, true);
    EXPECT_TRUE(lt.has_neighbours());

    Address addr_b;
    addr_b.mid({2, 2, 2, 2, 2, 2});
    LongPositionVector pv_b;
    pv_b.gn_addr = addr_b;
    lt.update(pv_b);

    lt.is_neighbour(addr_a, false);
    EXPECT_FALSE(lt.has_neighbours());

    lt.is_neighbour(addr_a, true);
    lt.is_neighbour(addr_b, true);
    auto neighbours = lt.neighbours();
    EXPECT_EQ(2, std::distance(neighbours.begin(), neighbours.end()));
    EXPECT_TRUE(std::any_of(neighbours.begin(), neighbours.end(),
                [&pv_a](const LocationTable::entry_type& e) {
                    return e.position == pv_a;
                }));
    EXPECT_TRUE(std::any_of(neighbours.begin(), neighbours.end(),
                [&pv_b](const LocationTable::entry_type& e) {
                    return e.position == pv_b;
                }));

    lt.is_neighbour(addr_a, false);
    neighbours = lt.neighbours();
    EXPECT_EQ(1, std::distance(neighbours.begin(), neighbours.end()));
    EXPECT_EQ(pv_b, neighbours.begin()->position);
}

TEST(LocationTable, is_duplicate_packet_timestamp) {
    LocationTable lt(cMIB);
    Address a;
    a.mid({1, 1, 1, 1, 1, 1});
    EXPECT_FALSE(lt.is_duplicate_packet(a, Timestamp(3 * abs_ms)));
    EXPECT_FALSE(lt.is_duplicate_packet(a, Timestamp(3 * abs_ms)));
    EXPECT_TRUE(lt.is_duplicate_packet(a, Timestamp(2 * abs_ms)));
    EXPECT_FALSE(lt.is_duplicate_packet(a, Timestamp(3 * abs_ms)));
    EXPECT_FALSE(lt.is_duplicate_packet(a, Timestamp(4 * abs_ms)));
    EXPECT_TRUE(lt.is_duplicate_packet(a, Timestamp(3 * abs_ms)));

    Address b;
    b.mid({2, 2, 2, 2, 2, 2});
    EXPECT_FALSE(lt.is_duplicate_packet(b, Timestamp(3 * abs_ms)));
}

TEST(LocationTable, is_duplicate_packet_sequence) {
    LocationTable lt(cMIB);
    Address a;
    a.mid({1, 1, 1, 1, 1, 1});
    EXPECT_FALSE(lt.is_duplicate_packet(a, SequenceNumber(3), Timestamp(8 * abs_ms)));
    EXPECT_TRUE(lt.is_duplicate_packet(a, SequenceNumber(3), Timestamp(8 * abs_ms)));
    EXPECT_FALSE(lt.is_duplicate_packet(a, SequenceNumber(3), Timestamp(9 * abs_ms)));
    EXPECT_TRUE(lt.is_duplicate_packet(a, SequenceNumber(3), Timestamp(9 * abs_ms)));
    EXPECT_FALSE(lt.is_duplicate_packet(a, SequenceNumber(4), Timestamp(9 * abs_ms)));
}

TEST(LocationTable, update_pdr) {
    LocationTable lt(cMIB);
    Address addr;
    addr.mid({1, 0, 1, 0, 1, 0});

    EXPECT_DOUBLE_EQ(0.0, lt.get_pdr(addr));
    lt.update_pdr(addr, 30, Timestamp(100 * abs_ms));
    EXPECT_DOUBLE_EQ(0.0, lt.get_pdr(addr));
    lt.update_pdr(addr, 10, Timestamp(200 * abs_ms));
    EXPECT_DOUBLE_EQ(50.0, lt.get_pdr(addr));
    lt.update_pdr(addr, 480, Timestamp(1200 * abs_ms));
    EXPECT_DOUBLE_EQ(265.0, lt.get_pdr(addr));
    lt.update_pdr(addr, 312, Timestamp(1800 * abs_ms));
    EXPECT_DOUBLE_EQ(392.5, lt.get_pdr(addr));
}

TEST(LocationTable, expire) {
    LocationTable lt(cMIB);
    Timestamp clock(0 * abs_ms);
    const auto almost_expire =
        Timestamp::duration_type(cMIB.itsGnLifetimeLocTE) -
        1000 * Timestamp::millisecond();
    const auto jiffy_expire = 2000 * Timestamp::millisecond();

    LongPositionVector pv;
    Address addr;
    addr.mid({3, 7, 3, 7, 3, 7});
    pv.gn_addr = addr;

    lt.update(pv);
    EXPECT_TRUE(lt.has_entry(addr));
    lt.expire(clock);
    EXPECT_TRUE(lt.has_entry(addr));
    clock += almost_expire;
    lt.expire(clock);
    EXPECT_TRUE(lt.has_entry(addr));
    clock += jiffy_expire;
    lt.expire(clock);
    EXPECT_FALSE(lt.has_entry(addr));

    lt.update(pv);
    EXPECT_TRUE(lt.has_entry(addr));
    clock += almost_expire;
    lt.expire(clock);
    EXPECT_TRUE(lt.has_entry(addr));
    lt.update(pv); // no refresh with same PV
    clock += jiffy_expire;
    lt.expire(clock);
    EXPECT_FALSE(lt.has_entry(addr));

    lt.update(pv);
    clock += almost_expire;
    lt.expire(clock);
    EXPECT_TRUE(lt.has_entry(addr));
    pv.timestamp += 20 * Timestamp::millisecond();
    lt.update(pv); // refresh with updated PV
    clock += jiffy_expire;
    lt.expire(clock);
    EXPECT_TRUE(lt.has_entry(addr));
}

