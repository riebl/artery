import sys

from importlib import reload
from warnings import catch_warnings


for mod in ['vanetza', '_vanetza']:
    sys.modules.pop(mod, None)

with catch_warnings(record=True) as w:
    import vanetza.net as vanetza_net
    reload(vanetza_net)

    assert not len(w), 'expected vanetza stub imports to pass without any error'


from vanetza.net import ByteBufferConvertible, CohesivePacket, ChunkPacket, OsiLayer, \
    osi_layer_iterable, max_osi_layer, min_osi_layer, num_osi_layers, osi_layer_list


def test_buffer_convertable():
    data = 'hello, world!'.encode()
    buffer = ByteBufferConvertible(data)

    assert buffer.size() == len(data), f'expected buffer and bytes object sizes to match, got {len(data)} != {buffer.size()}'
    
    copied_data = (
        buffer.convert(),
        buffer.convert(),
        buffer.convert()
    )

    for i, sample in enumerate(copied_data):
        others = copied_data[:i] + copied_data[i + 1:]

        assert all(other is not sample for other in others), 'expected all objects to be distinct (convert() should yield copied buffers each time)'
        assert all(other == data for other in others), 'expected all objects to the same'


def test_cohesive_packet():
    msg = 'hello, world!'.encode()

    packet = CohesivePacket(msg, OsiLayer.Physical)
    assert packet.size_layer(OsiLayer.Physical) == len(msg)
    assert packet.size() == len(msg)
    assert packet[OsiLayer.Physical] == msg

    part1, part2, part3 = 6, 1, 6
    assert sum((part1, part2, part3)) == len(msg)

    packet.set_boundary(OsiLayer.Physical, part1)
    packet.set_boundary(OsiLayer.Link, part2)
    packet.set_boundary(OsiLayer.Network, part3)

    # total size should stay the same
    assert packet.size() == len(msg)

    assert packet.size_layer(OsiLayer.Physical) == part1
    assert packet.size_layer(OsiLayer.Link) == part2
    assert packet.size_layer(OsiLayer.Network) == part3

    assert packet.size_range(OsiLayer.Physical, OsiLayer.Physical) == part1
    assert packet.size_range(OsiLayer.Physical, OsiLayer.Link) == part1 + part2
    assert packet.size_range(OsiLayer.Physical, OsiLayer.Network) == part1 + part2 + part3

    assert packet.buffer() == msg

    packet.trim(OsiLayer.Physical, part1 + part2)
    assert packet.size() == len(msg[:part1 + part2])

    packet.trim(OsiLayer.Link, 0)
    assert packet.size() == part1


def test_osi_layer_aux():
    start, end = OsiLayer.Presentation, OsiLayer.Application
    diff = 2

    assert len(list(osi_layer_iterable(start, end))) == diff
    assert len(osi_layer_list(start, end)) == diff

    assert max_osi_layer() == OsiLayer.Application
    assert min_osi_layer() == OsiLayer.Physical

    assert num_osi_layers(start, end) == diff


def test_chunk_packet():
    def yield_size(iterable):
        return map(lambda p: len(p), iterable)
    
    def yield_encoded(iterable):
        return map(lambda p: p.encode(), iterable)

    part1, part2, part3 = yield_encoded(('hello', ' ', 'world!'))

    packet = ChunkPacket()
    for layer, buffer in zip(
        osi_layer_list(OsiLayer.Physical, OsiLayer.Network), (part1, part2, part3)
    ):
        packet[layer] = ByteBufferConvertible(buffer)

    assert packet.size() == sum(yield_size((part1, part2, part3)))
    assert packet.size_range(OsiLayer.Physical, OsiLayer.Link) == sum(yield_size((part1, part2)))

    new_part3 = part3 + '!'.encode()
    packet.set_layer(OsiLayer.Network, ByteBufferConvertible(new_part3))
    
    assert packet[OsiLayer.Network].convert() == new_part3

    new_packet = packet.extract(OsiLayer.Physical, OsiLayer.Link)
    assert new_packet.size() == sum(yield_size((part1, part2)))

    part4, part5 = yield_encoded(('!', '!'))
    new_packet[OsiLayer.Presentation] = ByteBufferConvertible(part4)
    new_packet[OsiLayer.Application] = ByteBufferConvertible(part5)

    packet = packet.merge(new_packet, OsiLayer.Presentation, OsiLayer.Application)
    assert packet.size() == sum(yield_size((new_part3, part4, part5)))
