from vanetza.net import (
    ByteBufferConvertible
)


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
