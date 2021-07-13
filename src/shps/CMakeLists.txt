set(SHPS_SOURCES
		../../include/ssxtools/Core.h
		../../include/ssxtools/EndianSwap.h
		../../include/ssxtools/shps/ShpsReader.h
		../../include/ssxtools/shps/ShpsConverter.h
		ShpsReader.cpp
		ShpsConverter.cpp
		stbimpl.cpp
)

add_library(ssxtools-shps STATIC ${SHPS_SOURCES})

set_property(TARGET ssxtools-shps PROPERTY CXX_STANDARD 20)
set_property(TARGET ssxtools-shps PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(ssxtools-shps PUBLIC ${PROJECT_SOURCE_DIR}/vendor)
target_include_directories(ssxtools-shps PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# $ROOT/include is the public API for ssxtools-shps
target_include_directories(ssxtools-shps PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Link additional libraries required for using the 
# filesystem library with libc++ and libstdc++, if we need to.
if(SSXTOOLS_STDLIB STREQUAL "libc++")
	target_link_libraries(ssxtools-shps c++fs)
elseif(SSXTOOLS_STDLIB STREQUAL "libstdc++")
	target_link_libraries(ssxtools-shps stdc++fs)
endif()

target_link_libraries(ssxtools-shps mcommon bigfile)