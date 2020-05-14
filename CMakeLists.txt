cmake_minimum_required(VERSION 3.1)
project(EAGLe CXX)

include(CheckCXXSourceCompiles)
find_package(Threads)

# EAGLe is made up of two seperate components.
#
# SHPSCore - The core library that handles SHPS images.
# eagle-cli - The EAGLe CLI interface.
#
# This option is provided for other projects to allow importing SHPSCore without also importing the EAGLE UI projects (and causing clutter).
option(EAGLE_SHPSCORE_ONLY "Turn this option on if you want to use SHPSCore in your projects" OFF)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
endif()

# Detect the particular C++ standard library if it's not cached.
if(NOT DEFINED CACHE{EAGLE_STDLIB})
	message(STATUS "Detecting C++ standard library")
	set(CMAKE_REQUIRED_QUIET ON)

	check_cxx_source_compiles(
		"#include <ciso646>
		#if !defined(__GLIBCXX__)
		#error \"not libstdc++\"
		#endif
		int main() {}"
	__LIBRARY_LIBSTDCXX)

	check_cxx_source_compiles(
		"#include <ciso646>
		#if !defined(_LIBCPP_VERSION)
		#error \"not libc++\"
		#endif
		int main() {}"
	__LIBRARY_LIBCXX)

	check_cxx_source_compiles(
		"#include <ciso646>
		#if !defined(_MSVC_STL_VERSION)
		#error \"not MSVC\"
		#endif
		int main() {}"
	__LIBRARY_MSVC)

	if(__LIBRARY_LIBSTDCXX)
		set(STANDARD_LIBRARY "libstdc++")
	elseif(__LIBRARY_LIBCXX)
		set(STANDARD_LIBRARY "libc++")
	elseif(__LIBRARY_MSVC)
		set(STANDARD_LIBRARY "msvc")
	else()
		message(FATAL_ERROR "Can't find which C++ runtime is in use")
	endif()

	set(EAGLE_STDLIB ${STANDARD_LIBRARY} CACHE STRING "The C++ standard library in use; possible values: libstdc++, libc++, msvc")

	message(STATUS "Detected ${EAGLE_STDLIB}")
endif()

add_subdirectory(src/shpscore)

if(NOT EAGLE_SHPSCORE_ONLY)
	add_subdirectory(src/eaglecli)
endif()