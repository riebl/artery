cmake_minimum_required(VERSION 3.15)

project(comms VERSION 0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES CommunicationManager.cc)
set(HEADERS CommunicationManager.h SingletonHolder.h)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC cppzmq protos plog::plog)
target_include_directories(${PROJECT_NAME} PUBLIC ${ROOT_DIR})