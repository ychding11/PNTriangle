if(MSVC AND NOT "${MSVC_VERSION}" LESS 1400)
	add_definitions( "/MP" )
endif()


add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)


add_subdirectory (glfw-3.1.2)

include_directories(
	glfw-3.1.2/include/GLFW/
	glew-1.13.0/include/
)


###############################################################################

set( GLEW_SOURCE glew-1.13.0/src/glew.c )

add_library( GLEW_1130 STATIC ${GLEW_SOURCE} ${GLEW_INCLUDE} )

target_link_libraries(GLEW_1130 ${OPENGL_LIBRARY} ${EXTRA_LIBS} )


### ASSIMP ###
# AssImp already has a CMakeLists.txt so let's use these

# Compile built-in, modified version of Zlib
include(CheckIncludeFile)
include(CheckTypeSize)
include(CheckFunctionExists)
add_subdirectory( assimp-3.0.1270/contrib/zlib )

# Compile without Boost
include_directories( assimp-3.0.1270/code/BoostWorkaround )
add_definitions( -DASSIMP_BUILD_BOOST_WORKAROUND )

# Compile AssImp
set( LIB_INSTALL_DIR "lib")
set(LIBASSIMP_COMPONENT libassimp3.0-r1270-OGLtuts)
set(ZLIB_LIBRARIES zlib)
set(BUILD_STATIC_LIB ON)
#set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib)
add_subdirectory( assimp-3.0.1270/code )




