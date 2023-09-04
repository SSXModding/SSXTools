add_library(ssxtools_io
	src/WorldStreamReader.cpp
)

target_link_libraries(ssxtools_io PUBLIC
	ssxtools::core
	ssxtools::format
)

ssxtools_target(ssxtools_io)
add_library(ssxtools::io ALIAS ssxtools_io)
