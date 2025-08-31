add_library(transfusion-protos OBJECT)

protobuf_generate(
    TARGET transfusion-protos
    PROTOS TransfusionMsg.proto
)
artery_mark_as_codegen(transfusion-protos)

add_artery_feature(
    transfusion
    TransfusionLoopback.cc
    TransfusionService.cc
)

target_link_libraries(transfusion PRIVATE transfusion-protos)
