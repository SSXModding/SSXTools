set(WRITER_SOURCES
	writer.h
	writer.cpp
	stb_implementation.cpp
)

add_library(eaglewriter STATIC ${WRITER_SOURCES})

set_property(TARGET eaglewriter PROPERTY CXX_STANDARD 17)
set_property(TARGET eaglewriter PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(eaglewriter PUBLIC ${PROJECT_SOURCE_DIR}/vendor)
target_include_directories(eaglewriter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(eaglewriter shpscore)

# Link additional libraries required for using the 
# filesystem library with libc++ and libstdc++.
if(EAGLE_STDLIB STREQUAL "libc++")
	target_link_libraries(eaglewriter c++fs)
elseif(EAGLE_STDLIB STREQUAL "libstdc++")
	target_link_libraries(eaglewriter stdc++fs)
endif()

target_link_libraries(eaglewriter ${CMAKE_THREAD_LIBS_INIT})