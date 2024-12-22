cmake_minimum_required(VERSION 3.16)

project(comms VERSION 1.0 LANGUAGES CXX)

set(SOURCES )
set(HEADERS CommunicationManager.h SingletonHolder.h)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC cppzmq protos plog::plog)
target_include_directories(${PROJECT_NAME} PUBLIC ${ROOT_DIR})