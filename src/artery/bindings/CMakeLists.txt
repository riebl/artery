cmake_minimum_required(VERSION 3.16)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Python bindings are split into two parts:
# - embedded modules, that are only available during runtime
# - normal bindings, that are exported as standalone shared libraries

# Normal bindings have several advantages over their embedded counterparts,
# primarily being easy to test. Embedded modules could be tested only within
# fully constructed runtime, and may hardly provide coverage.

add_artery_feature(python_bindings)

pybind11_add_module(_vanetza
    VanetzaNetBindings.cc
)

target_sources(python_bindings PRIVATE
    PythonItsG5Service.cc
)

target_link_libraries(python_bindings PRIVATE pybind11::pybind11)
target_link_libraries(_vanetza PRIVATE vanetza)

target_compile_definitions(python_bindings PRIVATE
    $<$<NOT:$<CONFIG:Release>>:PREFER_LOCAL_PYTHON_STUBS>
    $<$<NOT:$<CONFIG:Release>>:PYTHON_BINDINGS_DIR="${CMAKE_CURRENT_BINARY_DIR}">
)

add_test(NAME test_python_bindings
    COMMAND ${Python3_EXECUTABLE} -m pytest -v tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set_tests_properties(test_python_bindings PROPERTIES
    ENVIRONMENT "PYTHONPATH=$PYTHONPATH:${CMAKE_CURRENT_BINARY_DIR}"
)
