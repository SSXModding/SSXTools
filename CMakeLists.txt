cmake_minimum_required(VERSION 3.1)

project(EAGLe CXX)

set(EAGLE_SOURCES
	src/main.cpp

	src/ShpsReader.h
	src/ShpsReader.cpp

	src/stb_implementation.cpp

	src/Util.h
	src/EndianSwap.h
)

add_executable(eagle ${EAGLE_SOURCES})

set_property(TARGET eagle PROPERTY CXX_STANDARD 17)
set_property(TARGET eagle PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(eagle PRIVATE ${PROJECT_SOURCE_DIR}/vendor)
target_include_directories(eagle PRIVATE ${PROJECT_SOURCE_DIR}/src)