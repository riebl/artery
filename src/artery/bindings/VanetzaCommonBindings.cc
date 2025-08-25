#include "TypeConvertions.h"

#include <pybind11/pybind11.h>
#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/byte_buffer_convertible.hpp>
#include <vanetza/common/byte_order.hpp>

namespace py = pybind11;

/*
 * Modules that do not depend on embedded omnet/artery functionality
 * should be exported as shared libraries that can be used
 * outside of runtime for proper testing.
 */

namespace
{

template <typename T, vanetza::ByteOrder ORDER>
void declare_endian_type(py::module& m, const std::string& cls)
{
    using endian_type = vanetza::EndianType<T, ORDER>;
    /*
     * Endian convertions should be done by first initializing endian type
     * and then calling its respective functions. This class should be used only for convertions.
     */
    py::class_<endian_type>(m, cls.c_str()).def(py::init<T>()).def("net", &endian_type::net).def("host", &endian_type::host).def("get", &endian_type::get);
}

}  // namespace

PYBIND11_MODULE(_vanetza_net, m)
{
    // Python code should implement byte convertors itself
    py::class_<vanetza::ByteBufferConvertible>(m, "ByteBufferConvertible")
        .def(py::init())
        .def(py::init<const std::string&>())
        .def(py::init([](const py::bytes& bytes) { return vanetza::ByteBufferConvertible(artery::types::from_bytes(bytes)); }))
        .def(
            "convert",
            [](vanetza::ByteBufferConvertible& self) {
                vanetza::ByteBuffer buffer;
                self.convert(buffer);
                return artery::types::to_bytes(buffer);
            })
        .def("size", &vanetza::ByteBufferConvertible::size);

    declare_endian_type<std::uint8_t, vanetza::ByteOrder::BigEndian>(m, "uint8be_t");
    declare_endian_type<std::uint16_t, vanetza::ByteOrder::BigEndian>(m, "uint16be_t");
    declare_endian_type<std::uint32_t, vanetza::ByteOrder::BigEndian>(m, "uint32be_t");
    declare_endian_type<std::uint64_t, vanetza::ByteOrder::BigEndian>(m, "uint64be_t");

    declare_endian_type<std::int8_t, vanetza::ByteOrder::BigEndian>(m, "int8be_t");
    declare_endian_type<std::int16_t, vanetza::ByteOrder::BigEndian>(m, "int16be_t");
    declare_endian_type<std::int32_t, vanetza::ByteOrder::BigEndian>(m, "int32be_t");
    declare_endian_type<std::int64_t, vanetza::ByteOrder::BigEndian>(m, "int64be_t");
}
