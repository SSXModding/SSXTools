add_library(ssxtools_format INTERFACE)
ssxtools_header_only_target(ssxtools_format)

target_link_libraries(ssxtools_format INTERFACE
	ssxtools::core
)

add_library(ssxtools::format ALIAS ssxtools_format)
