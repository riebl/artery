# Binds: extern/vanetza/vanetza/net/chunk_packet.hpp
# pylint: disable=unused-import,function-redefined,useless-parent-delegation

from copy import deepcopy
from typing import Optional, List

from vanetza.common.byte_buffer_convertable import ByteBufferConvertible
from vanetza.net.osi_layer import OsiLayer, osi_layer_iterable, max_osi_layer, min_osi_layer

try:
    from _vanetza import ChunkPacket

except ImportError:
    import warn

    class ChunkPacket:
        def __init__(self, other: Optional['ChunkPacket'] = None):
            """
            Args:
                other (Optional[ChunkPacket]): Packet to copy. Defaults to None.
            """
            if other is not None:
                self.__layers = deepcopy(other.__layers)
            else:
                self.__layers: List[ByteBufferConvertible] = [
                    ByteBufferConvertible() for _ in osi_layer_iterable(min_osi_layer(), max_osi_layer())
                ]

        def layer(self, ol: OsiLayer) -> ByteBufferConvertible:
            """
            Access specific Osi Layer of a packet.

            Args:
                ol (OsiLayer): a layer to access.

            Returns:
                ByteBufferConvertible: reference to binary contents of that layer.
            """
            return self.__layers[ol]

        def __getitem__(self, key: OsiLayer) -> ByteBufferConvertible:
            """
            Same as layer().

            Args:
                key (OsiLayer): a layer to access.

            Returns:
                ByteBufferConvertible: reference to binary contents of that layer.
            """
            return self.__layers[key]

        def size(self) -> int:
            """
            Returns total size of a packet.

            Returns:
                int: size in bytes.
            """
            return sum([layer.size() for layer in self.__layers])

        def size_range(self, start: OsiLayer, end: OsiLayer) -> int:
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

        def extract(self, start: OsiLayer, end: OsiLayer) -> 'ChunkPacket':
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
            for layer in osi_layer_iterable(start, end):
                self[layer] = source[layer]
                source = ByteBufferConvertible()
            return self
