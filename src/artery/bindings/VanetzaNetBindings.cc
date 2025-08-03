#include "TypeConvertions.h"

#include <pybind11/pybind11.h>
#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/byte_buffer_convertible.hpp>
#include <vanetza/net/chunk_packet.hpp>
#include <vanetza/net/cohesive_packet.hpp>
#include <vanetza/net/osi_layer.hpp>
#include <vanetza/net/packet.hpp>

#include <cstddef>
#include <cstring>

namespace py = pybind11;

/*
 * Modules that do not depend on embedded omnet/artery functionality
 * should be exported as shared libraries that can be used
 * outside of runtime for proper testing.
 */

PYBIND11_MODULE(_vanetza_net, m)
{
    py::enum_<vanetza::OsiLayer>(m, "OsiLayer")
        .value("Physical", vanetza::OsiLayer::Physical)
        .value("Link", vanetza::OsiLayer::Link)
        .value("Network", vanetza::OsiLayer::Network)
        .value("Transport", vanetza::OsiLayer::Transport)
        .value("Session", vanetza::OsiLayer::Session)
        .value("Presentation", vanetza::OsiLayer::Presentation)
        .value("Application", vanetza::OsiLayer::Application);

    m.def("min_osi_layer", vanetza::min_osi_layer);
    m.def("max_osi_layer", vanetza::max_osi_layer);

    m.def("distance", vanetza::distance);
    m.def("num_osi_layers", vanetza::num_osi_layers);

    // num_osi_layers and osi_layer_range are wrapped as python code

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

    py::class_<vanetza::ChunkPacket>(m, "ChunkPacket")
        .def(py::init())
        .def(py::init<const vanetza::ChunkPacket&>())
        .def(
            "layer", static_cast<vanetza::ByteBufferConvertible& (vanetza::ChunkPacket::*)(vanetza::OsiLayer)>(&vanetza::ChunkPacket::layer),
            py::return_value_policy::reference_internal)
        .def("set_layer", [](vanetza::ChunkPacket& self, vanetza::OsiLayer ol, const vanetza::ByteBufferConvertible& buffer) { self.layer(ol) = buffer; })
        .def(
            "__getitem__", static_cast<vanetza::ByteBufferConvertible& (vanetza::ChunkPacket::*)(vanetza::OsiLayer)>(&vanetza::ChunkPacket::operator[]),
            py::return_value_policy::reference_internal)
        .def("__setitem__", [](vanetza::ChunkPacket& self, vanetza::OsiLayer ol, const vanetza::ByteBufferConvertible& buffer) { self[ol] = buffer; })
        .def("size", static_cast<std::size_t (vanetza::ChunkPacket::*)() const>(&vanetza::ChunkPacket::size))
        .def("size_range", static_cast<std::size_t (vanetza::ChunkPacket::*)(vanetza::OsiLayer, vanetza::OsiLayer) const>(&vanetza::ChunkPacket::size))
        .def("extract", &vanetza::ChunkPacket::extract, py::return_value_policy::take_ownership)
        .def("merge", &vanetza::ChunkPacket::merge, py::return_value_policy::reference_internal);

    py::class_<vanetza::CohesivePacket>(m, "CohesivePacket")
        .def(py::init<const vanetza::CohesivePacket&>())
        .def(py::init([](const py::bytes& bytes, vanetza::OsiLayer layer) { return vanetza::CohesivePacket(artery::types::from_bytes(bytes), layer); }))
        .def(
            "__getitem__",
            [](vanetza::CohesivePacket& self, vanetza::OsiLayer layer) {
                vanetza::CohesivePacket::buffer_range range = self[layer];
                return artery::types::to_memview(range);
            })
        .def("set_boundary", &vanetza::CohesivePacket::set_boundary)
        .def("trim", &vanetza::CohesivePacket::trim)
        .def("size", static_cast<std::size_t (vanetza::CohesivePacket::*)() const>(&vanetza::CohesivePacket::size))
        .def("size_layer", static_cast<std::size_t (vanetza::CohesivePacket::*)(vanetza::OsiLayer) const>(&vanetza::CohesivePacket::size))
        .def("size_range", static_cast<std::size_t (vanetza::CohesivePacket::*)(vanetza::OsiLayer, vanetza::OsiLayer) const>(&vanetza::CohesivePacket::size))
        .def("buffer", [](vanetza::CohesivePacket& self) { return artery::types::to_bytes(self.buffer()); });
}
