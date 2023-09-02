add_library(ssxtools_core
	src/StreamUtils.cpp
	src/EacRiffChunker.cpp
)

ssxtools_target(ssxtools_core)

add_library(ssxtools::core ALIAS ssxtools_core)
