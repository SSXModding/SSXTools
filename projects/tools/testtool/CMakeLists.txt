add_executable(testtool
	main.cpp
)

ssxtools_target(testtool)

target_link_libraries(testtool
	ssxtools::core
)
