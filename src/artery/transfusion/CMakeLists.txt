add_artery_feature(
    transfusion
    TransfusionLoopback.cc
    TransfusionService.cc
)

target_link_libraries(transfusion PRIVATE protobuf::libprotobuf)

protobuf_generate(
    TARGET transfusion
    LANGUAGE cpp
    PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/TransfusionMsg.proto
    PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(transfusion PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
