# Binds: extern/vanetza/vanetza/net/chunk_packet.hpp
# pylint: disable=unused-import,function-redefined,useless-parent-delegation

from copy import deepcopy
from typing import Optional, List, TYPE_CHECKING

from vanetza.common.byte_buffer_convertable import ByteBufferConvertible
from vanetza.net.osi_layer import _OsiLayer, osi_layer_iterable, max_osi_layer, _min_osi_layer


class _ChunkPacket:
    def __init__(self, other: Optional['ChunkPacket'] = None):
        """
        Args:
            other (Optional[ChunkPacket]): Packet to copy. Defaults to None.
        """
        if other is not None:
            self.__layers = deepcopy(other.__layers)
        else:
            self.__layers: List[ByteBufferConvertible] = [
                ByteBufferConvertible() for _ in osi_layer_iterable(_min_osi_layer(), max_osi_layer())
            ]

    def layer(self, ol: _OsiLayer) -> ByteBufferConvertible:
        """
        Access specific Osi Layer of a packet.

        Args:
            ol (OsiLayer): a layer to access.

        Returns:
            ByteBufferConvertible: reference to binary contents of that layer.
        """
        return self.__layers[ol]
    
    def set_layer(self, ol: _OsiLayer, buffer: ByteBufferConvertible):
        """
        Sets buffer to provided Osi Layer.

        Args:
            ol (OsiLayer): layer to assign data to.
            buffer (ByteBufferConvertible): data to assign.
        """
        self.__layers[ol] = buffer

    def __getitem__(self, key: _OsiLayer) -> ByteBufferConvertible:
        """
        Same as layer().

        Args:
            key (OsiLayer): a layer to access.

        Returns:
            ByteBufferConvertible: reference to binary contents of that layer.
        """
        return self.layer(key)
    
    def __setitem__(self, key: _OsiLayer, value: ByteBufferConvertible):
        """
        Same as set_layer().

        Args:
            key (OsiLayer): layer to assign data to.
            value (ByteBufferConvertible): data to assign.
        """
        self.set_layer(key, value)

    def size(self) -> int:
        """
        Returns total size of a packet.

        Returns:
            int: size in bytes.
        """
        return sum([layer.size() for layer in self.__layers])

    def size_range(self, start: _OsiLayer, end: _OsiLayer) -> int:
        """
        Returns size of packet in Osi layers [start, end].

        Args:
            start (OsiLayer): beginning of range. 
            end (OsiLayer): end of range, inclusive.

        Returns:
            int: size in bytes.
        """
        s = 0
        for layer in osi_layer_iterable(start, end):
            s += self[layer].size()
        return s

    def extract(self, start: _OsiLayer, end: _OsiLayer) -> 'ChunkPacket':
        """
        Extract a range of Osi layers to a new packet.

        Args:
            start (OsiLayer): beginning of range. 
            end (OsiLayer): end of range, inclusive.

        Returns:
            ChunkPacket: newly created packet with extracted contents.
        """
        new = ChunkPacket()
        for layer in osi_layer_iterable(start, end):
            new[layer] = self[layer]
            self[layer] = ByteBufferConvertible()
        return new

    def merge(self, source: 'ChunkPacket', start: _OsiLayer, end: _OsiLayer) -> 'ChunkPacket':
        """
        Merges a range of Osi layers into this packet.

        Args:
            source (ChunkPacket): source package to take data from.
            start (OsiLayer): beginning of range. 
            end (OsiLayer): end of range, inclusive.

        Returns:
            ChunkPacket: reference to self.
        """
        for layer in osi_layer_iterable(start, end):
            self[layer] = source[layer]
            source = ByteBufferConvertible()
        return self


if TYPE_CHECKING:
    ChunkPacket = _ChunkPacket
else:
    try:
        from _vanetza import ChunkPacket
    except ImportError:
        import warn
        ChunkPacket = _ChunkPacket    
