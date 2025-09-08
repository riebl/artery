add_protobuf(transfusion-protos PROTOS TransfusionMsg.proto)

add_artery_feature(
    transfusion
    TransfusionLoopback.cc
    TransfusionService.cc
)

target_link_libraries(transfusion PRIVATE transfusion-protos)
