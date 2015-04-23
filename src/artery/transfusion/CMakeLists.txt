find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS TransfusionMsg.proto)
add_library(transfusion OBJECT TransfusionService.cc ${PROTO_SRCS} ${PROTO_HDRS})
set_target_properties(transfusion PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    INCLUDE_DIRECTORIES "$<TARGET_PROPERTY:artery,INCLUDE_DIRECTORIES>;${CMAKE_CURRENT_BINARY_DIR}")
