find_package(PythonInterp 3 REQUIRED)

macro(import_opp_target _target _opp_makefile)
    # _cmake_target: generated CMake file with import target [optional argument]
    if(${ARGC} GREATER 2)
        set(_cmake_target "${ARGV2}")
    else()
        set(_cmake_target "${PROJECT_BINARY_DIR}/${_target}-targets.cmake")
    endif()

    # opp_makemake generated Makefile is required for proceeding
    if(NOT EXISTS ${_opp_makefile})
        message(FATAL_ERROR "Cannot import ${_target} because there is no opp_makemake file at ${_opp_makefile}")
    endif()

    # generate target file (prior to build system generation)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} opp_cmake.py ${_opp_makefile} ${_cmake_target}
        ERROR_VARIABLE _opp_cmake_error
        RESULT_VARIABLE _opp_cmake_result
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    if(_opp_cmake_result)
        message(STATUS "opp_cmake generated following error log")
        message(SEND_ERROR ${_opp_cmake_error})
        message(FATAL_ERROR "import_opp_target for ${_target} aborted due to above opp_cmake error")
    endif()

    # include import targets if generated successfully
    if(EXISTS ${_cmake_target})
        include(${_cmake_target})
    else()
        message(FATAL_ERROR "There is no ${_cmake_target} for OMNeT++ import of ${_target}")
    endif()

    # sanity check: included target file has to provide the expected target
    if(NOT TARGET ${_target})
        message(FATAL_ERROR "Import of target ${_target} from ${_opp_makefile} failed")
    endif()
endmacro()
