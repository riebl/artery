#include <gtest/gtest.h>
#include <vanetza/common/lru_cache.hpp>

using namespace vanetza;

class LruCacheTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        generator_calls = 0;
        last_key = 0;
    }

    std::function<int(int)> generator()
    {
        return [this](int key) {
            ++generator_calls;
            last_key = key;
            return ~key;
        };
    }

    unsigned generator_calls;
    int last_key;
};


TEST_F(LruCacheTest, caching)
{
    LruCache<int, int> cache(generator(), 5);

    // generate one new entry
    EXPECT_EQ(~8, cache[8]);
    EXPECT_EQ(1, generator_calls);

    // refer to cached entry
    EXPECT_EQ(~8, cache[8]);
    EXPECT_EQ(1, generator_calls);

    // further entry
    EXPECT_EQ(~4, cache[4]);
    EXPECT_EQ(2, generator_calls);

    // first entry is still there
    EXPECT_EQ(~8, cache[8]);
    EXPECT_EQ(2, generator_calls);
}

TEST_F(LruCacheTest, modify)
{
    LruCache<int, int> cache(generator(), 5);

    // modify new cache entry
    cache[8] = 23;
    EXPECT_EQ(23, cache[8]);

    // modify existing entry
    cache[8] = 42;
    EXPECT_EQ(42, cache[8]);

    EXPECT_EQ(1, generator_calls);
}

TEST_F(LruCacheTest, drop_lru)
{
    LruCache<int, int> cache(generator(), 3);
    cache[1];
    cache[2];
    cache[3];
    EXPECT_EQ(3, generator_calls);

    EXPECT_EQ(~2, cache[2]); // LRU: 1, 3, 2
    cache[4]; // drop 1, LRU: 3, 2, 4
    cache[1]; // re-create entry, LRU: 2, 4, 1
    EXPECT_EQ(5, generator_calls);

    EXPECT_EQ(~2, cache[2]);
    EXPECT_EQ(~4, cache[4]);
    EXPECT_EQ(~1, cache[1]);
    EXPECT_EQ(5, generator_calls);
}
