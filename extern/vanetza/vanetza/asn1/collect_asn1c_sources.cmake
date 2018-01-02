if(NOT DEFINED ASN1C_OUTPUT_DIR)
    message(FATAL_ERROR "Missing ASN.1 output directory (ASN1C_OUTPUT_DIR)")
elseif(NOT DEFINED ASN1C_SOURCE_FILE)
    message(FATAL_ERROR "Missing ASN.1 source file (ASN1C_SOURCE_FILE)")
endif()

file(GLOB _sources RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${ASN1C_OUTPUT_DIR}/*.c)
list(REMOVE_ITEM _sources ${ASN1C_OUTPUT_DIR}/converter-sample.c ${ASN1C_OUTPUT_DIR}/converter-example.c)

file(WRITE "${ASN1C_SOURCE_FILE}" "# generated file")
foreach(_source ${_sources})
    file(APPEND "${ASN1C_SOURCE_FILE}" "${_source}\n")
endforeach()

