cmake_minimum_required(VERSION 3.10...3.27)

project(GamePlay)

set(GAMEPLAY_VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)


# debug
message( "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}" )
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG" )
    add_definitions(-D_DEBUG)
endif()

# architecture
if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
set(ARCH_DIR "x64")
else()
set(ARCH_DIR "x86")
endif()

if (ANDROID)
    set(ARCH ${ANDROID_ARCH_NAME})
elseif(NOT IOS)
    # Skip any introspection for cross-compiling targets
    include(${PROJECT_SOURCE_DIR}/cmake/TargetArch.cmake)

    set(ARCH "unknown")
    target_architecture(ARCH)
endif(ANDROID)

string(TOLOWER ${CMAKE_SYSTEM_NAME} LOWER_SYSTEM_NAME)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/out")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/out/lib/${LOWER_SYSTEM_NAME}/${ARCH}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})

file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH})
message(STATUS "Directory: ${LIBRARY_OUTPUT_PATH}")

if (GP_NO_PLATFORM)
add_definitions(-DGP_NO_PLATFORM=1)
endif (GP_NO_PLATFORM)

if (GP_NO_LUA_BINDINGS)
add_definitions(-DGP_NO_LUA_BINDINGS=1)
endif (GP_NO_LUA_BINDINGS)

# gameplay library
add_subdirectory(gameplay)

# gameplay samples
#add_subdirectory(samples)

# gameplay encoder
# A pre-compiled executable can be found in 'gameplay/bin'. Uncomment to build yourself.
#add_subdirectory(tools/encoder)
#add_subdirectory(tools/luagen)
