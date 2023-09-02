function(ssxtools_target target)
    target_compile_definitions(${target} PRIVATE "$<$<CONFIG:DEBUG>:SSXTOOLS_DEBUG>")

	# public headers reside with a component (if they exist)
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include)
    	target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
	endif()

    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(${target} PUBLIC cxx_std_20)

    # some sane compiler flags
    set(_CORE_COMPILE_ARGS -Wall -Wextra)
    set(_CORE_LINKER_ARGS "")

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        set(_CORE_COMPILE_ARGS ${_CORE_COMPILE_ARGS} -Werror)

        # If on Release use link-time optimizations.
        # On clang we use ThinLTO for even better build performance.
        if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
            set(_CORE_COMPILE_ARGS ${_CORE_COMPILE_ARGS} -flto=thin)
            set(_CORE_LINKER_ARGS ${_CORE_LINKER_ARGS} -flto=thin)
            target_link_options(${target} PRIVATE -fuse-ld=${SSXTOOLS_LINKER} -flto=thin)
        else()
            set(_CORE_COMPILE_ARGS ${_CORE_COMPILE_ARGS} -flto)
            set(_CORE_LINKER_ARGS ${_CORE_LINKER_ARGS} -flto)
            target_link_options(${target} PRIVATE -fuse-ld=${SSXTOOLS_LINKER} -flto)
        endif()

    endif()

    if("asan" IN_LIST SSXTOOLS_BUILD_FEATURES)
        # Error if someone's trying to mix asan and tsan together,
        # they aren't compatible.
        if("tsan" IN_LIST SSXTOOLS_BUILD_FEATURES)
            message(FATAL_ERROR "ASAN and TSAN cannot be used together.")
        endif()

        message(STATUS "Enabling ASAN for target ${target}")
        set(_CORE_COMPILE_ARGS ${_CORE_COMPILE_ARGS} -fsanitize=address)
        set(_CORE_LINKER_ARGS ${_CORE_LINKER_ARGS} -fsanitize=address)
    endif()

    if("tsan" IN_LIST SSXTOOLS_BUILD_FEATURES)
        message(STATUS "Enabling TSAN for target ${target}")
        set(_CORE_COMPILE_ARGS ${_CORE_COMPILE_ARGS} -fsanitize=thread)
        set(_CORE_LINKER_ARGS ${_CORE_LINKER_ARGS} -fsanitize=thread)
    endif()

    if("ubsan" IN_LIST SSXTOOLS_BUILD_FEATURES)
        message(STATUS "Enabling UBSAN for target ${target}")
        set(_CORE_COMPILE_ARGS ${_CORE_COMPILE_ARGS} -fsanitize=undefined)
        set(_CORE_LINKER_ARGS ${_CORE_LINKER_ARGS} -fsanitize=undefined)
    endif()

    target_compile_options(${target} PRIVATE ${_CORE_COMPILE_ARGS})
    target_link_options(${target} PRIVATE ${_CORE_LINKER_ARGS})
endfunction()

function(ssxtools_header_only_target target)
	target_include_directories(${target} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_compile_features(${target} INTERFACE cxx_std_20)
endfunction()

function(ssxtools_set_alternate_linker)
    find_program(LINKER_EXECUTABLE ld.${SSXTOOLS_LINKER} ${SSXTOOLS_LINKER})
    if(LINKER_EXECUTABLE)
        message(STATUS "Using ${SSXTOOLS_LINKER} as linker for ssxtools projects")
    else()
        message(FATAL_ERROR "Linker ${SSXTOOLS_LINKER} does not exist on your system. Please specify one which does or omit this option from your configure command.")
    endif()
endfunction()
