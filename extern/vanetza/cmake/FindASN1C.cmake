find_program(ASN1C_EXECUTABLE NAMES asn1c DOC "ASN.1 compiler")
if(ASN1C_EXECUTABLE)
    execute_process(COMMAND ${ASN1C_EXECUTABLE} -version ERROR_VARIABLE _asn1c_version)
    string(REGEX MATCH "[0-9]\\.[0-9]\\.[0-9]+" ASN1C_VERSION ${_asn1c_version})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(asn1c
    REQUIRED_VARS ASN1C_EXECUTABLE
    FOUND_VAR ASN1C_FOUND
    VERSION_VAR ASN1C_VERSION)

mark_as_advanced(ASN1C_EXECUTABLE)
