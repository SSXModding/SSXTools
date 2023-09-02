add_library(ssxtools_core
	src/StreamUtils.cpp
	src/EacRiffStream.cpp
)

ssxtools_target(ssxtools_core)

add_library(ssxtools::core ALIAS ssxtools_core)
