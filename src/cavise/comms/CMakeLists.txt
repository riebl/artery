cmake_minimum_required(VERSION 3.16)

set(SOURCES )
list(APPEND SOURCES SingletonHolder.h)
list(APPEND SOURCES CommunicationManager.h)

target_sources(${PROJECT_NAME} PUBLIC ${SOURCES})
