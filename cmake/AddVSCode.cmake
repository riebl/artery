function(generate_vscode)
    set(one_value_args "TARGET;FILE")
    cmake_parse_arguments(args "${option_args}" "${one_value_args}" "" ${ARGN})

    if(args_UNPARSED_ARGUMENTS)
        message(SEND_ERROR "generate_vscode called with invalid arguments: ${args_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT args_TARGET)
        message(SEND_ERROR "generate_vscode: TARGET argument is missing")
    endif()

    if(NOT args_FILE)
        message(SEND_ERROR "generate_vscode: FILE argument is missing")
    endif()

    # backup existing launch.json
    if(EXISTS ${args_FILE})
        string(TIMESTAMP t)
        file(COPY_FILE ${args_FILE} ${args_FILE}.${t})
    endif()

    # collect all NED folders for given target
    get_ned_folders(${args_TARGET} opp_run_ned_folders)
    set(opp_run_ned_folders "\"-n$<JOIN:${opp_run_ned_folders},:>\"")

    # collect libraries for opp_run
    set(opp_run_libraries "")
    _get_opp_run_dependencies(${args_TARGET} opp_run_dependencies)
    foreach(opp_run_dependency IN LISTS opp_run_dependencies)
        list(APPEND opp_run_libraries "\"-l$<TARGET_FILE:${opp_run_dependency}>\"")
    endforeach()
    set(opp_run_libraries "$<JOIN:${opp_run_libraries},,>")

    set(opp_run_executable ${OMNETPP_RUN_DEBUG})

    # substitute variables first, then generator expressions
    configure_file(${PROJECT_SOURCE_DIR}/cmake/launch.json.in ${args_FILE} @ONLY)
    file(GENERATE OUTPUT ${args_FILE} INPUT ${args_FILE})
endfunction()
