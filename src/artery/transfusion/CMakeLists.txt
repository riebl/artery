add_library(transfusion-protos OBJECT)
protobuf_generate(
    TARGET transfusion-protos
    LANGUAGE cpp
    PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/TransfusionMsg.proto
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
)
artery_mark_as_codegen(transfusion-protos)

add_artery_feature(
    transfusion
    TransfusionLoopback.cc
    TransfusionService.cc
)

target_include_directories(transfusion PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(transfusion PUBLIC transfusion-protos)
