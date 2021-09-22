set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(SSXTOOLS_UI_SOURCES
	main.cpp

	AboutUi.h
	AboutUi.cpp

	MainWindow.h
	MainWindow.cpp

	ShapeWidget.cpp

	# QT/win32 things
	eagle.qrc
	MainWindow.ui
	AboutUi.ui
	eagle.rc
)

add_executable(ssxtools WIN32 ${SSXTOOLS_UI_SOURCES})

target_include_directories(ssxtools PRIVATE ${PROJECT_BINARY_DIR})
# fixes clang screaming about microsoft include rules
target_include_directories(ssxtools PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_property(TARGET ssxtools PROPERTY CXX_STANDARD 20)
set_property(TARGET ssxtools PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(ssxtools ssxtools-shps Qt5::Widgets)

install(TARGETS ssxtools RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}")

if(WIN32)
# Install the Qt libraries into the installation prefix.

message(STATUS "Adding install code to deploy Qt libraries into install prefix.")
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/temp_ssxtools_path" CONTENT "$<TARGET_FILE:ssxtools>")

set(WINDEPLOYQT_ARGUMENTS
		--dry-run
		--no-translations
		--no-system-d3d-compiler
		--no-angle
		--no-opengl-sw
		--list mapping)

# Allow windeployqt to install libgcc/libstdc++/libwinpthread, for MinGW-w64 builds.
if(NOT MSVC)
	set(WINDEPLOYQT_ARGUMENTS ${WINDEPLOYQT_ARGUMENTS}
		--compiler-runtime
	)
endif()

install(CODE
	"
	file(READ \"${CMAKE_CURRENT_BINARY_DIR}/temp_ssxtools_path\" _file)

	execute_process(
		COMMAND \"${CMAKE_COMMAND}\" -E
			env PATH=\"${_qt_bin_dir}\" \"${WINDEPLOYQT_EXECUTABLE}\"
				${WINDEPLOYQT_ARGUMENTS}
				\${_file}
		OUTPUT_VARIABLE _output
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	separate_arguments(_files WINDOWS_COMMAND \${_output})
	while(_files)
		list(GET _files 0 _src)
		list(GET _files 1 _dest)
		message(STATUS \"Copying QT/system library \${_src} -> \${_dest} \")
		execute_process(
			COMMAND \"${CMAKE_COMMAND}\" -E
				copy \${_src} \"\${CMAKE_INSTALL_PREFIX}/\${_dest}\"
		)
		list(REMOVE_AT _files 0 1)
	endwhile()
	"
)

endif()