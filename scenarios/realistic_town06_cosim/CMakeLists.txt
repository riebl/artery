cmake_minimum_required(VERSION 3.15)

# Найдите и укажите необходимые библиотеки и пакеты
find_package(Protobuf REQUIRED VERSION 3.18.1 CONFIG)
find_package(cppzmq REQUIRED VERSION 4.10.0 CONFIG)
find_package(plog REQUIRED VERSION 1.1.10 CONFIG)

# Генерация кода protobuf
set(PROTO_SOURCE_DIR messages)

set(PROTOS )
list(APPEND PROTOS ${PROTO_SOURCE_DIR}/artery.proto)
list(APPEND PROTOS ${PROTO_SOURCE_DIR}/opencda.proto)

set(PROTO_BINARY_DIR $ENV{PROTO_PATH})
set(PROTO_GENERATOR_DIR ${PROTO_BINARY_DIR}/cavise)

if("${PROTO_BINARY_DIR}" EQUAL "")
    message(FATAL_ERROR "PROTO_PATH was not specified. Are you sure that environment was activated?")
endif()

add_library(protos OBJECT ${PROTOS})
file(MAKE_DIRECTORY ${PROTO_GENERATOR_DIR})

protobuf_generate(
    TARGET protos 
    IMPORT_DIRS ${PROTO_SOURCE_DIR} 
    PROTOC_OUT_DIR ${PROTO_GENERATOR_DIR}
    LANGUAGE cpp)

target_compile_options(protos PUBLIC -fPIC)

# generate for python separately
protobuf_generate(
    TARGET protos 
    IMPORT_DIRS ${PROTO_SOURCE_DIR} 
    PROTOC_OUT_DIR ${PROTO_GENERATOR_DIR}
    LANGUAGE python)
    
add_artery_feature(ArteryManager ArteryManager.cc)

target_include_directories(protos PUBLIC 
    ${PROTO_BINARY_DIR}
    # include libprotobuf headers to override system libprotobuf if present
    ${protobuf_INCLUDE_DIRS}
)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Линковка необходимых библиотек
target_link_libraries(ArteryManager PRIVATE protobuf::libprotobuf libzmq-static protos comms plog::plog)

# Добавление цели запуска в OMNeT++
add_opp_run(realistic_town06_cosim CONFIG omnetpp.ini)
