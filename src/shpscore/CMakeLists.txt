# TODO This should be a generic library

set(SHPS_SOURCES
	Util.h
	EndianSwap.h
	
	ShpsReader.h
	ShpsReader.cpp
)

add_library(shpscore STATIC ${SHPS_SOURCES})

set_property(TARGET shpscore PROPERTY CXX_STANDARD 17)
set_property(TARGET shpscore PROPERTY CXX_STANDARD_REQUIRED ON)

# Publicize includes here as they're the public API for SHPSCore.
target_include_directories(shpscore PUBLIC ${PROJECT_SOURCE_DIR}/vendor)
target_include_directories(shpscore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link additional libraries required for using the 
# filesystem library with libc++ and libstdc++.
if(EAGLE_STDLIB STREQUAL "libc++")
	target_link_libraries(shpscore c++fs)
elseif(EAGLE_STDLIB STREQUAL "libstdc++")
	target_link_libraries(shpscore stdc++fs)
endif()

target_link_libraries(shpscore ${CMAKE_THREAD_LIBS_INIT})