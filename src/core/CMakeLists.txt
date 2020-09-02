# TODO This should be a generic library

set(SHPS_SOURCES
	Core.h
	EndianSwap.h
	
	RefPack.h
	RefPack.cpp

	readers/ShpsReader.h
	readers/ShpsReader.cpp
	
	serializers/ShpsWriter.h
	serializers/ShpsWriter.cpp
	serializers/stbimpl.cpp
)

add_library(eagle-core STATIC ${SHPS_SOURCES})

set_property(TARGET eagle-core PROPERTY CXX_STANDARD 17)
set_property(TARGET eagle-core PROPERTY CXX_STANDARD_REQUIRED ON)

# Publicize includes here as they're the public API for SHPSCore.
target_include_directories(eagle-core PUBLIC ${PROJECT_SOURCE_DIR}/vendor)
target_include_directories(eagle-core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link additional libraries required for using the 
# filesystem library with libc++ and libstdc++.
if(EAGLE_STDLIB STREQUAL "libc++")
	target_link_libraries(eagle-core c++fs)
elseif(EAGLE_STDLIB STREQUAL "libstdc++")
	target_link_libraries(eagle-core stdc++fs)
endif()

target_link_libraries(eagle-core ${CMAKE_THREAD_LIBS_INIT})