include(UseGTest)
prepare_gtest(common)

add_gtest(BitNumber bit_number.cpp)
add_gtest(ByteBuffer byte_buffer.cpp)
add_gtest(ByteBufferSink byte_buffer_sink.cpp)
add_gtest(ByteBufferSource byte_buffer_source.cpp)
add_gtest(ByteView byte_view.cpp)
add_gtest(Hook hook.cpp)
add_gtest(Runtime runtime.cpp)

