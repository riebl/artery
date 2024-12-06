cmake_minimum_required(VERSION 3.16)

project(comms VERSION 1.0 LANGUAGES CXX)

find_package(Protobuf REQUIRED VERSION 3.18.1 CONFIG)
find_package(cppzmq REQUIRED VERSION 4.10.0 CONFIG)
find_package(plog REQUIRED VERSION 1.1.10 CONFIG)

set(SOURCES )
set(HEADERS CommunicationManager.h SingletonHolder.h)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC cppzmq protos plog::plog)
target_include_directories(${PROJECT_NAME} PUBLIC ${ROOT_DIR})