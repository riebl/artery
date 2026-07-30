################
# Bridge tests #
################

add_sionna_test(test_bridge_module
    TEST_NAME
        test_bridge_module
    FILES
        TestCapabilities.cc
        TestCasters.cc
        TestBridgeConstants.cc
        TestBridgeMaterial.cc
        TestBridgeScene.cc
        TestBridgeSceneObject.cc
        TestDefaulted.cc
)

target_link_libraries(test_bridge_module
    PRIVATE
        sionna-bridge-basic-llvm-ad
)

set_tests_properties(test_bridge_module
    PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}:$ENV{PYTHONPATH}"
)
