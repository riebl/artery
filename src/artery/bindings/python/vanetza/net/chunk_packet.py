# Binds: extern/vanetza/vanetza/net/chunk_packet.hpp
# pylint: disable=unused-import,function-redefined,useless-parent-delegation

from copy import deepcopy
from typing import Optional, List, TYPE_CHECKING

from vanetza.net.byte_buffer_convertible import ByteBufferConvertible
from vanetza.net.osi_layer import OsiLayer, osi_layer_iterable, max_osi_layer, min_osi_layer


class _ChunkPacket:
    def __init__(self, other: Optional['ChunkPacket'] = None):
        """
        Args:
            other (Optional[ChunkPacket]): Packet to copy. Defaults to None.
        """
        ...

    def layer(self, ol: OsiLayer) -> ByteBufferConvertible:
        """
        Access specific Osi Layer of a packet.

        Args:
            ol (OsiLayer): a layer to access.

        Returns:
            ByteBufferConvertible: reference to binary contents of that layer.
        """
        ...
    
    def set_layer(self, ol: OsiLayer, buffer: ByteBufferConvertible):
        """
        Sets buffer to provided Osi Layer.

        Args:
            ol (OsiLayer): layer to assign data to.
            buffer (ByteBufferConvertible): data to assign.
        """
        ...

    def __getitem__(self, key: OsiLayer) -> ByteBufferConvertible:
        """
        Same as layer().

        Args:
            key (OsiLayer): a layer to access.

        Returns:
            ByteBufferConvertible: reference to binary contents of that layer.
        """
        ...
    
    def __setitem__(self, key: OsiLayer, value: ByteBufferConvertible):
        """
        Same as set_layer().

        Args:
            key (OsiLayer): layer to assign data to.
            value (ByteBufferConvertible): data to assign.
        """
        ...

    def size(self) -> int:
        """
        Returns total size of a packet.

        Returns:
            int: size in bytes.
        """
        ...

    def size_range(self, start: OsiLayer, end: OsiLayer) -> int:
        """
        Returns size of packet in Osi layers [start, end].

        Args:
            start (OsiLayer): beginning of range. 
            end (OsiLayer): end of range, inclusive.

        Returns:
            int: size in bytes.
        """
        ...

    def extract(self, start: OsiLayer, end: OsiLayer) -> 'ChunkPacket':
        """
        Extract a range of Osi layers to a new packet.

        Args:
            start (OsiLayer): beginning of range. 
            end (OsiLayer): end of range, inclusive.

        Returns:
            ChunkPacket: newly created packet with extracted contents.
        """
        ...

    def merge(self, source: 'ChunkPacket', start: OsiLayer, end: OsiLayer) -> 'ChunkPacket':
        """
        Merges a range of Osi layers into this packet.

        Args:
            source (ChunkPacket): source package to take data from.
            start (OsiLayer): beginning of range. 
            end (OsiLayer): end of range, inclusive.

        Returns:
            ChunkPacket: reference to self.
        """
        ...


if TYPE_CHECKING:
    ChunkPacket = _ChunkPacket
else:
    try:
        from _vanetza import ChunkPacket
    except ImportError:
        import warn
        ChunkPacket = _ChunkPacket    
