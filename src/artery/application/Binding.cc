#include "pybind11/detail/common.h"
#include "pybind11/pybind11.h"
#include "vanetza/common/byte_buffer.hpp"
#include "vanetza/common/byte_buffer_convertible.hpp"
#include "vanetza/net/chunk_packet.hpp"
#include "vanetza/net/cohesive_packet.hpp"
#include "vanetza/net/osi_layer.hpp"

#include <pybind11/embed.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <vanetza/net/packet.hpp>

#include <cstddef>

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(_omnetpp, m)
{
}

PYBIND11_EMBEDDED_MODULE(_vanetza, m)
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

    // TODO: wrap num_osi_layers and osi_layer_range as python code
    // (should be better than binding directly)

    py::class_<vanetza::ByteBufferConvertible>(m, "ByteBufferConvertible")
        .def(py::init())
        .def(py::init<std::string>())
        .def(py::init<vanetza::ByteBuffer>())
        .def(
            "convert",
            [](vanetza::ByteBufferConvertible& self) {
                vanetza::ByteBuffer buffer;
                self.convert(buffer);
                return buffer;
            })
        .def("size", &vanetza::ByteBufferConvertible::size);

    py::class_<vanetza::ChunkPacket>(m, "ChunkPacket")
        .def(py::init())
        .def(py::init<vanetza::ChunkPacket&>())
        .def(
            "layer", static_cast<vanetza::ByteBufferConvertible& (vanetza::ChunkPacket::*)(vanetza::OsiLayer)>(&vanetza::ChunkPacket::layer),
            py::return_value_policy::reference_internal)
        .def(
            "__getitem__", static_cast<vanetza::ByteBufferConvertible& (vanetza::ChunkPacket::*)(vanetza::OsiLayer)>(&vanetza::ChunkPacket::operator[]),
            py::return_value_policy::reference_internal)
        .def("size", static_cast<std::size_t (vanetza::ChunkPacket::*)() const>(&vanetza::ChunkPacket::size))
        .def("size_range", static_cast<std::size_t (vanetza::ChunkPacket::*)(vanetza::OsiLayer, vanetza::OsiLayer) const>(&vanetza::ChunkPacket::size))
        .def("extract", &vanetza::ChunkPacket::extract, py::return_value_policy::take_ownership)
        .def("merge", &vanetza::ChunkPacket::merge, py::return_value_policy::reference_internal);
}


PYBIND11_EMBEDDED_MODULE(_artery, m)
{
}