# Generates launch.json for VS Code, containing the default debug configuration. If PROJECT_SOURCE_DIR already
# contains a .vscode/launch.json, the default debug configuration is appended.
#
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

    # substitute variables first, then generator expressions, save in build directory
    configure_file(${PROJECT_SOURCE_DIR}/cmake/launch.json.in ${PROJECT_BINARY_DIR}/launch.json.tmp @ONLY)
    file(GENERATE OUTPUT ${PROJECT_BINARY_DIR}/launch.json.default INPUT ${PROJECT_BINARY_DIR}/launch.json.tmp)

    # if launch.json exists, find generated configuration and modify it
    if(EXISTS ${args_FILE})
        set(default_config_found false)
        file(READ ${args_FILE} current_launch_json)

        string(JSON num_configs LENGTH ${current_launch_json} "configurations")
        message(STATUS "Found ${num_configs} VS Code Config(s) in .vscode/launch.json")
        math(EXPR last_config "${num_configs} - 1")

        # read recent build configuration from temporary launch configuraiton
        file(READ ${PROJECT_BINARY_DIR}/launch.json.default default_launch_json)

        # get generated default configuration
        string(JSON default_launch_configuration GET ${default_launch_json} "configurations" 0)

        # get the default configuration name
        string(JSON default_launch_configuration_name GET ${default_launch_json} "configurations" 0 "name")

        # get inputs required by default configuration
        string(JSON default_launch_inputs GET ${default_launch_json} "inputs")

        # iterate available configs and search for the default config
        foreach(config_id RANGE ${last_config})
            string(JSON config_name GET ${current_launch_json} "configurations" ${config_id} name)

            # found Arterys default debug configuration
            if (${config_name} STREQUAL ${default_launch_configuration_name})
                message(STATUS "Updating Artery default config in .vscode/launch.json")
                set(default_config_found true)

                # replace old config with newly generated one and save
                string(JSON current_launch_json SET ${current_launch_json} "configurations" ${config_id} ${default_launch_configuration})
                file(WRITE ${args_FILE} ${current_launch_json})
            endif()
        endforeach()

        # launch.json is present but no default configuration was found
        if (NOT ${default_config_found})
            message(STATUS "Append Artery default config to .vscode/launch.json")

            # append default configuration
            string(JSON current_launch_configuration SET ${current_launch_json} "configurations" ${num_configs} ${default_launch_configuration})

            # append inputs to already existing inputs
            string(JSON current_num_inputs LENGTH ${current_launch_configuration} "inputs")
            string(JSON default_num_inputs LENGTH ${default_launch_inputs})
            math(EXPR last_default_input "${default_num_inputs} - 1")

            foreach(default_input_index RANGE ${last_default_input})
                string(JSON input GET ${default_launch_inputs} ${default_input_index})
                math(EXPR current_input_index "${current_num_inputs} + ${default_input_index}")
                string(JSON current_launch_configuration SET ${current_launch_configuration} "inputs" ${current_input_index} ${input} )
            endforeach()

            file(WRITE ${args_FILE} ${current_launch_configuration})
        endif()
    else()
        message(STATUS "Generating default debug config for VS Code")
        # substitute variables first, then generator expressions
        file(GENERATE OUTPUT ${args_FILE} INPUT ${PROJECT_BINARY_DIR}/launch.json.default)
    endif()


endfunction()
