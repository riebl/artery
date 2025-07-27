# Binds: extern/vanetza/vanetza/net/cohesive_packet.hpp
# pylint: disable=unused-import,function-redefined,useless-parent-delegation

from typing import Optional, TYPE_CHECKING

from vanetza.net.osi_layer import OsiLayer, OsiLayers, max_osi_layer
from vanetza.net.byte_buffer_convertible import ByteBufferConvertible


class _CohesivePacket:
    def __init__(self, data: Optional[bytes] = None, layer: Optional[OsiLayer] = None):
        """
        Construct a cohesive packet. If data and layer are provided, they will
        initialize the buffer with all data belonging to the specified layer.

        Args:
            data (Optional[bytes]): Raw buffer bytes to assign to one layer.
            layer (Optional[OsiLayer]): OSI layer to assign all bytes to.
        """
        ...

    def __getitem__(self, layer: OsiLayer) -> memoryview:
        """
        Access a specific OSI layer's data as a memoryview.

        Args:
            layer (OsiLayer): Requested OSI layer.

        Returns:
            memoryview: View of underlying bytes corresponding to that layer.
        """
        ...

    def set_boundary(self, layer: OsiLayer, bytes: int):
        """
        Define where the specified layer ends.

        Args:
            layer (OsiLayer): Layer for which to set boundary.
            bytes (int): Number of bytes in that layer.
        """
        ...

    def trim(self, from_layer: OsiLayer, bytes: int):
        """
        Trim total packet size starting from a given layer.

        Args:
            from_layer (OsiLayer): Layer from which trimming begins.
            bytes (int): Final desired total byte length.
        """
        ...

    def size(self) -> int:
        """
        Total size of the packet.

        Returns:
            int: Number of bytes.
        """
        ...

    def size_layer(self, layer: OsiLayer) -> int:
        """
        Size of a single OSI layer.

        Args:
            layer (OsiLayer): Layer to query.

        Returns:
            int: Number of bytes.
        """
        ...

    def size_range(self, start: OsiLayer, end: OsiLayer) -> int:
        """
        Size of a contiguous range of OSI layers.

        Args:
            start (OsiLayer): First layer.
            end (OsiLayer): Last layer (inclusive).

        Returns:
            int: Number of bytes in range.
        """
        ...

    def buffer(self) -> bytes:
        """
        Access raw byte buffer copy of the packet.

        Returns:
            bytes: Copied raw packet buffer.
        """
        ...


if TYPE_CHECKING:
    CohesivePacket = _CohesivePacket
else:
    try:
        from _vanetza import CohesivePacket
    except ImportError:
        CohesivePacket = _CohesivePacket
        import warn
