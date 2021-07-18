add_library(ssxtools-core STATIC
		IoStreamReadStream.cpp
)

#target_include_directories(ssxtools-core PUBLIC ${PROJECT_SOURCE_DIR}/vendor)
target_include_directories(ssxtools-core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# $ROOT/include is the public API
target_include_directories(ssxtools-core PUBLIC ${PROJECT_SOURCE_DIR}/include)

# this alias should be used by all projects,
# (also gearing up for modern CMake export targets)
add_library(SSXTools::Core ALIAS ssxtools-core)


