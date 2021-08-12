
cmake_minimum_required (VERSION 3.10 FATAL_ERROR)

project ( PNTriangle-OGL VERSION 1.0.0 LANGUAGES C CXX )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message( STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER_VERSION})" )
message( STATUS "target system: ${CMAKE_SYSTEM_NAME} (${CMAKE_SYSTEM_VERSION})" )
message( STATUS "host system: ${CMAKE_HOST_SYSTEM_NAME} (${CMAKE_HOST_SYSTEM_VERSION})" )


find_package (OpenGL REQUIRED)

# Compile external dependencies
add_subdirectory (external)

option(VERBOSE_MODE "cmake verbose mode" ON)

#######################################################################################
## function definition
#######################################################################################
function(group_files src_files)
    foreach(f ${src_files})
        # replace "a/b/c" --> "a\\b\\c"
        string(REPLACE / \\\\ f ${f})
        # Get the path of the file relative to ${CMAKE_SOURCE_DIR}
        file(RELATIVE_PATH relative_path ${CMAKE_SOURCE_DIR} ${f})
        # Extract the folder, remove the filename from path 
        string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" relative_path ${relative_path})

        set(SRCGR "${relative_path}")
        # Source_group expects \\ (double antislash), not / (slash)
        string(REPLACE / \\\\ SRCGR ${SRCGR})
        #message(STATUS "1. group name=${SRCGR}, file=${f}")

        # regular expression https://cmake.org/cmake/help/latest/command/string.html#regex-specification
        if ("${f}" MATCHES ".*\\.glsl$")
           set(SRCGR "${SRCGR}\\\\glsl")
        endif()    

        if (VERBOSE_MODE)
           message(STATUS "2. group name=${SRCGR}, file=${f}")
        endif (VERBOSE_MODE)

        source_group(${SRCGR} FILES ${f})
    endforeach()
endfunction()

include_directories(
	.
	common/
	external/glew-1.13.0/include/
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
)

include_directories( ${CMAKE_SOURCE_DIR}/external/imgui )
include_directories( ${CMAKE_SOURCE_DIR}/external/fonts )

set(ALL_LIBS ${OPENGL_LIBRARY} glfw GLEW_1130)

add_definitions(
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB Model_3D_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Model/*.obj
    ${CMAKE_CURRENT_SOURCE_DIR}/Model/*.mtl
)
foreach(file ${Model_3D_FILES})
    file(COPY ${file}  DESTINATION Model/)
    message(STATUS "copy ${file} into build/Model") 
endforeach()

file(GLOB SHADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.glsl)
file( COPY ${SHADER_FILES} DESTINATION shaders/ )

file(GLOB SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/common/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB EXT_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/*.cpp
)


group_files("${SOURCE_FILES}")
group_files("${EXT_FILES}")
group_files("${SHADER_FILES}")

set(TARGET_NAME "PNTriangle")
add_executable( ${TARGET_NAME} ${SOURCE_FILES} ${SHADER_FILES} ${EXT_FILES} )
target_link_libraries( ${TARGET_NAME} ${ALL_LIBS} )

foreach(shader ${SHADER_FILES})
    set_source_files_properties( ${shader} PROPERTIES HEADER_FILE_ONLY TRUE )
    # if no build happens, no copy
    add_custom_command( TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${shader} shaders/ )
    message(STATUS "copy ${shader} into build/shaders") 
endforeach()
