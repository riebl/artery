message(STATUS "Patching asn_system.h")

file(READ asn_system.h CONTENT)
string(REPLACE "#ifndef	ASSUMESTDTYPES" "#include <stdint.h>" CONTENT ${CONTENT})
string(REPLACE "#endif	/* ASSUMESTDTYPES */" "" CONTENT ${CONTENT})
string(REPLACE "typedef" "// typedef" CONTENT ${CONTENT})
string(REPLACE "#define	ilogb	_logb" "" CONTENT ${CONTENT})
file(WRITE asn_system.h ${CONTENT})
