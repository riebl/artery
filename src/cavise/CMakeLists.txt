####################
# CAVISE SUBMODULE #
####################

# This file is entrypoint for common CAVISE tools & enhancements to Artery.
cmake_minimum_required(VERSION 3.19)

set(PROTO_SOURCE_DIR messages)

set(PROTOS )
list(APPEND PROTOS ${PROTO_SOURCE_DIR}/artery.proto)
list(APPEND PROTOS ${PROTO_SOURCE_DIR}/opencda.proto)

set(PROTO_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protos)
set(PROTO_GENERATOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protos/cavise)

add_library(protos OBJECT ${PROTOS})
protobuf_generate(
    TARGET protos 
    IMPORT_DIRS ${PROTO_SOURCE_DIR} 
    PROTOC_OUT_DIR ${PROTO_GENERATOR_DIR}
    LANGUAGE cpp)

target_compile_options(protos PUBLIC -fPIC)
target_include_directories(protos PUBLIC 
    ${PROTO_BINARY_DIR}
    # include libprotobuf headers to override system libprotobuf if present
    ${protobuf_INCLUDE_DIRS}
)

add_subdirectory(comms)