# Currently dead

set(SSXTOOLSCLI_SOURCES
	main.cpp
)

add_executable(ssxtools-cli ${SSXTOOLSCLI_SOURCES})

set_property(TARGET ssxtools-cli PROPERTY CXX_STANDARD 17)
set_property(TARGET ssxtools-cli PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(ssxtools-cli ssxtools-shps)


install(TARGETS ssxtools-cli RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}")