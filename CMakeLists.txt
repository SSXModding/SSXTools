cmake_minimum_required(VERSION 3.14)

project(SSXTools
	LANGUAGES C CXX
	)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(Policies)
include(ProjectFuncs)

# required system dependencies
find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

# default linker
if(NOT SSXTOOLS_LINKER AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(SSXTOOLS_LINKER "lld")
elseif(NOT SSXTOOLS_LINKER)
    set(SSXTOOLS_LINKER "bfd")
endif()

ssxtools_set_alternate_linker()

add_subdirectory(projects/libraries/core)
add_subdirectory(projects/libraries/format)
add_subdirectory(projects/libraries/io)

add_subdirectory(projects/tools/testtool)

