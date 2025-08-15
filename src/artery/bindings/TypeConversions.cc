#include "TypeConvertions.h"

using namespace artery;

pybind11::memoryview types::to_memview(boost::iterator_range<vanetza::ByteBuffer::iterator>& range)
{
    auto size = std::distance(range.begin(), range.end());
    return pybind11::memoryview::from_memory(range.begin().base(), size);
}

pybind11::bytes types::to_bytes(const vanetza::ByteBuffer& buffer)
{
    // bytes owns underlying memory, so we need to copy here
    auto data = new char[buffer.size()];
    std::memcpy(data, buffer.data(), buffer.size());
    return pybind11::bytes(data, buffer.size());
}

vanetza::ByteBuffer types::from_bytes(const pybind11::bytes& bytes)
{
    pybind11::buffer buf = bytes;
    pybind11::buffer_info bufInfo = buf.request();
    vanetza::ByteBuffer buffer(bufInfo.size);

    std::copy(static_cast<std::uint8_t*>(bufInfo.ptr), static_cast<std::uint8_t*>(bufInfo.ptr) + bufInfo.size, buffer.begin());
    return buffer;
}
