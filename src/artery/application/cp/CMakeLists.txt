####################################
# Library of the CP Message Object #
####################################

add_artery_feature(cpm)

target_sources(cpm PRIVATE
    CpObject.cc
)


###################################
# Actual CpService implementation #
###################################

add_artery_feature(cpservice)

target_sources(cpservice PRIVATE
    CpService.cc
    GenerationRule.cc
    RedundancyMitigationRule.cc
    Tracking.cc
    Utility.cc
)

target_link_libraries(cpservice PUBLIC cpm)
target_link_libraries(cpservice PUBLIC envmod)
