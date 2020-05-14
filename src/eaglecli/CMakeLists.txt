set(EAGLECLI_SOURCES
	main.cpp
)

add_executable(eagle-cli ${EAGLECLI_SOURCES})

set_property(TARGET eagle-cli PROPERTY CXX_STANDARD 17)
set_property(TARGET eagle-cli PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(eagle-cli shpscore eaglewriter)