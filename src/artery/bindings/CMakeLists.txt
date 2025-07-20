cmake_minimum_required(VERSION 3.16)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

add_artery_feature(python_bindings)

target_sources(python_bindings PRIVATE
    PythonItsG5Service.cc
    VanetzaNetBindings.cc
)

target_link_libraries(python_bindings PRIVATE pybind11)

target_compile_definitions(python_bindings PRIVATE
    $<$<CONFIG:Debug>:PREFER_LOCAL_PYTHON_STUBS>
)

add_custom_target(test-bindings
    COMMAND ${Python3_EXECUTABLE} -m pytest -v
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "python bindings: tests"
)

add_test(NAME test_python_bindings
    COMMAND ${Python3_EXECUTABLE} -m pytest -v tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
