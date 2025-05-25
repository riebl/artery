# Binds: /workspaces/artery/extern/vanetza/vanetza/common/byte_buffer_convertible.hpp
# pylint: disable=unused-import,function-redefined,useless-parent-delegation

from typing import Optional, AnyStr

try:
    from _vanetza import ByteBufferConvertible

except ImportError:
    import warn

    class ByteBufferConvertible:
        """
        Serves as binary wrapper around any arbitry data in Vanetza.
        In Python services you should convert your data to AnyStr and pass to
        constructor of this class, instead of template overloads that are perfomed in C++ code.
        """

        def __init__(self, value: Optional[AnyStr]):
            if value is None:
                self.__value = bytes()
            elif isinstance(value, str):
                self.__value = value.encode()
            elif isinstance(value, bytes):
                self.__value = value
            else:
                raise TypeError
            
        def convert(self) -> bytes:
            """
            Converts underlying data to bytes and copies them to a different buffer.
            Use this to access binary data.

            Returns:
                bytes: a newly constructuted buffer with binary data.
            """
            return self.__value

        def size(self) -> int:
            """
            Gets size of byte array.

            Returns:
                int: byte length
            """
            return len(self.__value)
        