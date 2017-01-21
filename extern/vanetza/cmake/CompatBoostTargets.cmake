# Imported targets of Boost for compatibility with CMake prior version 3.5
if(NOT DEFINED Boost_FOUND)
    if(CMAKE_VERSION VERSION_LESS 3.5)
        message(SEND_ERROR "You are required to call find_package(Boost ...) beforehand!")
    else()
        message(AUTHOR_WARNING "Call find_package(Boost ...) beforehand for backward compatibilility")
    endif()
endif()

if(Boost_FOUND AND NOT TARGET Boost::boost)
    add_library(Boost::boost INTERFACE IMPORTED)
    set_target_properties(Boost::boost PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
endif()

# NOTE: every Boost library used by Vanetza has to be listed here
foreach(comp IN ITEMS date_time serialization system)
    string(TOUPPER "${comp}" COMP)
    if(Boost_${COMP}_FOUND AND NOT TARGET Boost::${comp})
        add_library(Boost::${comp} UNKNOWN IMPORTED)
        set_target_properties(Boost::${comp} PROPERTIES
            IMPORTED_LOCATION ${Boost_${COMP}_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
    endif()
endforeach()
