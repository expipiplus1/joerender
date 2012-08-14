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

set( joelang_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/joelang/include" )

set( glfw_INCLUDE_DIR    "${CMAKE_SOURCE_DIR}/glfw/include" )

set( glee_INCLUDE_DIR    "${CMAKE_SOURCE_DIR}/glee/include" )

#
# submodules
#

add_subdirectory( glfw EXCLUDE_FROM_ALL )

add_subdirectory( glee EXCLUDE_FROM_ALL )

add_subdirectory( joemath EXCLUDE_FROM_ALL )

add_subdirectory( joelang EXCLUDE_FROM_ALL )

set( UCPP_STAND_ALONE FALSE )
add_subdirectory( ucpp EXCLUDE_FROM_ALL )

set( joerender_LIBRARY_DIRS ${joelang_LIBRARY_DIRS} )
link_directories( ${joerender_LIBRARY_DIRS} )

#
# CXX flags
#

# because joemath is a header only library we include the flags here
set( joerender_CXX_FLAGS "${joemath_CXX_FLAGS} -Wall -std=c++11 -fno-exceptions -g" CACHE STRING "joerender compiler flags" )
set( joerender_CXX_FLAGS_RELEASE "${joemath_CXX_FLAGS_RELEASE} -Wall -std=c++11 -fno-exceptions -O4" CACHE STRING "joerender compiler flags" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${joerender_CXX_FLAGS}" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${joerender_CXX_FLAGS_RELEASE}" )

#
# Create the main engine
#

set( joerender_sources    src/main.cpp )

set( joerender_data_dir   data )

add_executable( joerender ${joerender_sources} )

include_directories( ${joemath_INCLUDE_DIR} ${joelang_INCLUDE_DIR} ${glfw_INCLUDE_DIR} ${glee_INCLUDE_DIR} )

set( GL_LIBS glfw libgleeStatic ${GLFW_LIBRARIES} )

target_link_libraries( joerender joelang ucpp ${GL_LIBS} )

#
# Various custom commands
#

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/data/clear_blue.jfx
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/data/clear_blue.jfx ${CMAKE_CURRENT_BINARY_DIR}/data/clear_blue.jfx
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/data/clear_blue.jfx )

add_custom_target( joerender_copy_files DEPENDS "${PROJECT_BINARY_DIR}/data/clear_blue.jfx" )
add_dependencies( joerender joerender_copy_files )

