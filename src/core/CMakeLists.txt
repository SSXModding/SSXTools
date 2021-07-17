add_library(ssxtools-core STATIC
		IoStreamReadStream.cpp
)

#target_include_directories(ssxtools-core PUBLIC ${PROJECT_SOURCE_DIR}/vendor)
target_include_directories(ssxtools-core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# $ROOT/include is the public API
target_include_directories(ssxtools-core PUBLIC ${PROJECT_SOURCE_DIR}/include)

