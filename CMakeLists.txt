cmake_minimum_required(VERSION 3.1)
project(SSXTools CXX)

include(CheckCXXSourceCompiles)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Threads)

option(SSXTOOLS_DISABLE_QT "Disable building the graphical user interface of SSXTools" OFF)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
endif()

# Detect the particular C++ standard library if it's not cached.
if(NOT DEFINED CACHE{SSXTOOLS_STDLIB})
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

	set(SSXTOOLS_STDLIB ${STANDARD_LIBRARY} CACHE STRING "The C++ standard library in use; possible values: libstdc++, libc++, msvc")

	message(STATUS "Detected C++ standard library \"${SSXTOOLS_STDLIB}\"")
endif()

set(BIGFILE_ENABLE_EXAMPLES OFF)
add_subdirectory(vendor/bigfile)

set(MCOMMON_DO_NOT_INSTALL ON)
add_subdirectory(vendor/mcommon)

add_subdirectory(src/core)
add_subdirectory(src/shps)


if(NOT SSXTOOLS_DISABLE_QT)
	add_subdirectory(src/gui)
endif()