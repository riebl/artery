protobuf_generate_cpp(
    PROTO_SRCS PROTO_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/TransfusionMsg.proto
)

add_library(
    transfusion-protos OBJECT
    ${PROTO_SRCS} ${PROTO_HDRS}
)

artery_mark_as_codegen(transfusion-protos)

target_link_libraries(transfusion-protos PUBLIC ${PROTOBUF_LIBRARIES})

target_include_directories(
    transfusion-protos PUBLIC
    ${PROTOBUF_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}
)

add_artery_feature(
    transfusion
    TransfusionLoopback.cc
    TransfusionService.cc
)

target_link_libraries(transfusion PRIVATE transfusion-protos)
