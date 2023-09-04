add_library(ssxtools_core
	src/StreamUtils.cpp

	# EAC-RIFF utilities
	src/EacRiffInStream.cpp
	src/EacRiffOutStream.cpp
)

ssxtools_target(ssxtools_core)

add_library(ssxtools::core ALIAS ssxtools_core)
