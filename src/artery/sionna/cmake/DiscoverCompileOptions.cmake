##########
# Uility #
##########

# Discovers compilation options (flags, definitions, linker flags, etc.)
# and populates `sionna-compilation-flags` with found settings. You may link
# with it to use discovered configuration.

include_guard(GLOBAL)

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag(-march=native HAVE_NATIVE_ARCH_SUPPORT)

set(SIONNA_CXX_COMPILER_FLAGS )
set(SIONNA_CXX_DEFINITIONS )

# This flag enables specific cpu instructions, which makes this code non-portable,
# but allows to compile with mitsuba.
if(NOT HAVE_NATIVE_ARCH_SUPPORT)
    message(WARNING "Your compiler does not support optimizing for current arch. This may break linkage with Mitsuba!")
else()
    list(APPEND SIONNA_CXX_COMPILER_FLAGS $<$<COMPILE_LANGUAGE:CXX>:-march=native>)
endif()

# Compilation with non-optimized backends (scalar*) is sort of unsupported
list(APPEND SIONNA_CXX_DEFINITIONS -DMI_ENABLE_LLVM=1)

# Compilation modes define what libraries are used with common sionna brdige code.
# For example, OMNeT++ uses special exceptions hierarchy to propagate errors: SB
# module may use these exceptions if SIONNA_BRIDGE_COMPILATION_MODE is set to OMNET.

# Use basic cpp only.
set(SIONNA_DEFAULT_COMPILATION_MODE SIONNA_BRIDGE_COMPILATION_MODE_BASIC)

# You may set custom SIONNA_BRIDGE_COMPILATION_MODE, if none set
# falls back to SIONNA_DEFAULT_COMPILATION_MODE.
set(_check_default_mode_set $<STREQUAL:$<TARGET_PROPERTY:SIONNA_BRIDGE_COMPILATION_MODE>,"">)

set(_check_mode_is_basic $<STREQUAL:$<TARGET_PROPERTY:SIONNA_BRIDGE_COMPILATION_MODE>,Basic>)
set(_check_mode_is_inet $<STREQUAL:$<TARGET_PROPERTY:SIONNA_BRIDGE_COMPILATION_MODE>,Inet>)

set(_choose_basic $<${_check_mode_is_basic}:SIONNA_BRIDGE_COMPILATION_MODE_BASIC>)
set(_choose_inet_or $<IF:${_check_mode_is_inet},SIONNA_BRIDGE_COMPILATION_MODE_INET,${_choose_basic}>)
set(_resolve_compilation_mode $<IF:${_check_default_mode_set},${SIONNA_DEFAULT_COMPILATION_MODE},${_choose_inet_or}>)

list(APPEND SIONNA_CXX_DEFINITIONS -D${_resolve_compilation_mode})

# Bridge resolve type dispatch is property-based.
# Do not define these macros when target properties are not set.
list(APPEND SIONNA_CXX_DEFINITIONS
    $<$<BOOL:$<TARGET_PROPERTY:SIONNA_FLOAT_TYPE>>:SIONNA_BRIDGE_RESOLVE_FLOAT=$<TARGET_PROPERTY:SIONNA_FLOAT_TYPE>>
    $<$<BOOL:$<TARGET_PROPERTY:SIONNA_SPECTRUM_TYPE>>:SIONNA_BRIDGE_RESOLVE_SPECTRUM=$<TARGET_PROPERTY:SIONNA_SPECTRUM_TYPE>>
)

# This variable hints Sionna bridge at virtual environment, as we know it
# during configuration phase. At runtime, Sionna should fall back to these if
# all other options fail.

# Don't use it too often, though.
set(SIONNA_VIRTUAL_ENV $ENV{VIRTUAL_ENV})

# We wrap variable into "" since it's a path.
list(APPEND SIONNA_CXX_DEFINITIONS SIONNA_VENV_HINT="${SIONNA_VIRTUAL_ENV}")

# Just populates discovered compiler options.
add_library(sionna-compilation-flags INTERFACE)

target_compile_options(sionna-compilation-flags INTERFACE ${SIONNA_CXX_COMPILER_FLAGS})
target_compile_definitions(sionna-compilation-flags INTERFACE ${SIONNA_CXX_DEFINITIONS})

target_compile_features(sionna-compilation-flags INTERFACE cxx_std_20)
