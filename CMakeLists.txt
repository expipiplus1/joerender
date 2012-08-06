cmake_minimum_required(VERSION 2.8)

#
# CMake Policies
#
cmake_policy(VERSION 2.8)

#
# SOURCE_GROUP
#
project( joerender )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++0x -fno-exceptions" )

#
# Uninstall operation
# defined here to stop any of the submodules defining it
#
if( NOT TARGET uninstall )
    configure_file( ${joerender_SOURCE_DIR}/cmake/uninstall.cmake.in
                    ${joerender_BINARY_DIR}/uninstall.cmake IMMEDIATE @ONLY )

    add_custom_target( uninstall
                       ${CMAKE_COMMAND} -P
                       ${joerender_BINARY_DIR}/uninstall.cmake )
endif( )

#
# load libraries
#
find_package( OpenGL REQUIRED )

set( joemath_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/joemath/include" )

set( glfw_INCLUDE_DIR    "${CMAKE_SOURCE_DIR}/glfw/include")

#
# submodules
#

add_subdirectory( glfw EXCLUDE_FROM_ALL )

#
# Create the main engine
#

set( joerender_sources    src/main.cpp )

set( joerender_data_dir data )

add_executable( joerender    ${joerender_sources} ) 

include_directories( ${joemath_INCLUDE_DIR} ${glfw_INCLUDE_DIR} )

set( GL_LIBS glfw ${GLFW_LIBRARIES} )

if( UNIX )
    if( NOT APPLE AND NOT CYGWIN )
        list( APPEND GL_LIBS rt X11 Xrandr Xxf86vm )
    elseif( APPLE )
        find_library( COCOA_FRAMEWORK Cocoa REQUIRED )
        list( APPEND GL_LIBS ${COCOA_FRAMEWORK} )
    endif()
elseif( WINDOWS )
    list( APPEND GL_LIBS opengl32 )
endif()

target_link_libraries( joerender ${joefx_LIBRARY} ${GL_LIBS} )
  
