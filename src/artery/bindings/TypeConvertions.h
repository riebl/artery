#pragma once

#include <boost/range/iterator_range.hpp>
#include <pybind11/pybind11.h>
#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/byte_buffer_convertible.hpp>
#include <vanetza/net/cohesive_packet.hpp>

namespace artery
{

namespace types
{

pybind11::memoryview to_memview(boost::iterator_range<vanetza::ByteBuffer::iterator>& range);

pybind11::bytes to_bytes(const vanetza::ByteBuffer& buffer);

vanetza::ByteBuffer from_bytes(const pybind11::bytes& bytes);

}  // namespace types

}  // namespace artery
