if(NOT DEFINED ASN1C_OUTPUT_DIR)
    message(FATAL_ERROR "Missing ASN.1 output directory (ASN1C_OUTPUT_DIR)")
endif()

file(GLOB _files ${ASN1C_OUTPUT_DIR}/*)
if(_files)
    file(REMOVE ${_files})
endif()
