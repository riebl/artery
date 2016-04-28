#include <vanetza/common/byte_sequence.hpp>
#include <algorithm>
#include <random>

namespace vanetza
{

ByteBuffer random_byte_sequence(std::size_t length, int seed)
{
    ByteBuffer buffer(length);
    std::generate(buffer.begin(), buffer.end(), random_byte_generator(seed));
    return buffer;
}

std::function<uint8_t()> random_byte_generator(int seed)
{
    std::mt19937 rng;
    rng.seed(seed);
    return [rng]() mutable { return rng(); };
}

} // namespace vanetza
