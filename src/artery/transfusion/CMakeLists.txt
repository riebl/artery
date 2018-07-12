find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS TransfusionMsg.proto)

add_artery_feature(transfusion
    TransfusionLoopback.cc
    TransfusionService.cc
    ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(transfusion PRIVATE ${PROTOBUF_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(transfusion PRIVATE ${PROTOBUF_LIBRARIES})
