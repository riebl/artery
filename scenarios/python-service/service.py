from vanetza.net.osi_layer import OsiLayer, osi_layer_list
from vanetza.net.chunk_packet import ChunkPacket, ByteBufferConvertible


class ExampleService:
    layers = osi_layer_list(OsiLayer.Session, OsiLayer.Application)

    def __init__(self):
        self.__packet = ChunkPacket()

    def initialize(self):
        data = ['hello', 'from', 'cav!']
        for ol, piece in zip(self.layers, data):
            self.__packet[ol] = ByteBufferConvertible(piece.encode())

    def finish(self):
        print(' '.join([self.__packet[ol].convert().decode() for ol in self.layers]))
