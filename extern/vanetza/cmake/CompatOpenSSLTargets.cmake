# Imported targets of OpenSSL for compatibility with CMake prior version 3.4
if(NOT DEFINED OPENSSL_FOUND)
    if(CMAKE_VERSION VERSION_LESS 3.4)
        message(SEND_ERROR "You are required to call find_package(OpenSSL ...) beforehand!")
    else()
        message(AUTHOR_WARNING "Call find_package(OpenSSL ...) beforehand for backward compatibilility")
    endif()
endif()

if(OPENSSL_FOUND)
    # Only OPENSSL_LIBRARIES existed before CMake 3.3
    if(NOT DEFINED OPENSSL_CRYPTO_LIBRARY OR NOT DEFINED OPENSSL_SSL_LIBRARY)
        if(NOT TARGET OpenSSL::SSL)
            add_library(OpenSSL::SSL INTERFACE IMPORTED)
            target_link_libraries(OpenSSL::SSL INTERFACE ${OPENSSL_LIBRARIES})
            target_include_directories(OpenSSL::SSL INTERFACE ${OPENSSL_INCLUDE_DIR})
        endif()
        if(NOT TARGET OpenSSL::Crypto)
            add_library(OpenSSL::Crypto ALIAS OpenSSL::SSL)
        endif()
    else()
        if(OPENSSL_SSL_LIBRARY AND NOT TARGET OpenSSL::SSL)
            add_library(OpenSSL::SSL UNKNOWN IMPORTED)
            set_target_properties(OpenSSL::SSL PROPERTIES
                IMPORTED_LOCATION ${OPENSSL_SSL_LIBRARY}
                INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
        endif()
        if(OPENSSL_CRYPTO_LIBRARY AND NOT TARGET OpenSSL::Crypto)
            add_library(OpenSSL::Crypto UNKNOWN IMPORTED)
            set_target_properties(OpenSSL::Crypto PROPERTIES
                IMPORTED_LOCATION ${OPENSSL_CRYPTO_LIBRARY}
                INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
        endif()
        if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            if(LIB_dl)
                # Conan's FindOpenSSL provides LIB_DL
                set(DYNAMIC_LOADING_LIBRARY ${LIB_dl})
            else()
                find_library(DYNAMIC_LOADING_LIBRARY NAME dl DOC "Dynamic loading library")
                mark_as_advanced(DYNAMIC_LOADING_LIBRARY)
            endif()
            if(DYNAMIC_LOADING_LIBRARY)
                set_property(TARGET OpenSSL::Crypto APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES ${DYNAMIC_LOADING_LIBRARY})
            endif()
        endif()
    endif()
endif()
