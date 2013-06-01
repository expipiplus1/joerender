cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

#-------------------------------------------------------------------------------
# Set up the project
#-------------------------------------------------------------------------------
project( joerender )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )


#-------------------------------------------------------------------------------
# Uninstall operation
# defined here to stop any of the submodules defining it
#-------------------------------------------------------------------------------
if( NOT TARGET uninstall )
    configure_file( ${joerender_SOURCE_DIR}/cmake/uninstall.cmake.in
                    ${joerender_BINARY_DIR}/uninstall.cmake IMMEDIATE @ONLY )

    add_custom_target( uninstall
                       ${CMAKE_COMMAND} -P
                       ${joerender_BINARY_DIR}/uninstall.cmake )
endif( )

#-------------------------------------------------------------------------------
# Grab all the required libraries
#-------------------------------------------------------------------------------

find_package( OpenGL REQUIRED )
list( APPEND JOERENDER_INTERNAL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR} )
list( APPEND JOERENDER_INTERNAL_LIBS ${OPENGL_gl_LIB} )

add_subdirectory( joemath EXCLUDE_FROM_ALL )
set( JOEMATH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/joemath/include" )
list( APPEND JOERENDER_INTERNAL_INCLUDE_DIRS ${JOEMATH_INCLUDE_DIR} )

add_subdirectory( glfw EXCLUDE_FROM_ALL )
set( GLFW_INCLUDE_DIR    "${CMAKE_SOURCE_DIR}/glfw/include" )
list( APPEND JOERENDER_INTERNAL_INCLUDE_DIRS ${GLFW_INCLUDE_DIR} )

add_subdirectory( glee EXCLUDE_FROM_ALL )
set( GLEE_INCLUDE_DIR    "${CMAKE_SOURCE_DIR}/glee/include" )
set( GLEE_LIB libgleeStatic )
list( APPEND JOERENDER_INTERNAL_INCLUDE_DIRS ${GLEE_INCLUDE_DIR} )

set( JOELANG_WITH_OPENGL ON )
add_subdirectory( joelang EXCLUDE_FROM_ALL )
get_property( JOELANG_INCLUDE_DIRS GLOBAL PROPERTY JOELANG_INCLUDE_DIRS )
get_property( JOELANG_LIBRARY_DIRS GLOBAL PROPERTY JOELANG_LIBRARY_DIRS )
get_property( JOELANG_LIBS GLOBAL PROPERTY JOELANG_LIBS )
list( APPEND JOERENDER_INTERNAL_INCLUDE_DIRS ${JOELANG_INCLUDE_DIRS} )
list( APPEND JOERENDER_LIBRARY_DIRS ${JOELANG_LIBRARY_DIRS} )
list( APPEND JOERENDER_LIBS ${JOELANG_LIBS} )

link_directories( ${JOERENDER_LIBRARY_DIRS} )

set( GL_LIBS glfw ${GLEE_LIB} ${GLFW_LIBRARIES} )
if( UNIX AND NOT APPLE )
    list( APPEND GL_LIBS Xi )
endif()
 
list( APPEND JOERENDER_LIBS ${GL_LIBS} )

add_definitions( -Wall )
add_definitions( -fno-exceptions )
set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11 )


#-------------------------------------------------------------------------------
# Create the main engine
#-------------------------------------------------------------------------------

set( joerender_sources    src/main.cpp )

set( joerender_data_dir   data )

include_directories( ${JOERENDER_INTERNAL_INCLUDE_DIRS} ${JOERENDER_INCLUDE_DIRS} )

if( APPLE )
    add_executable( joerender MACOSX_BUNDLE ${joerender_sources} )
    set_target_properties( joerender PROPERTIES MACOSX_BUNDLE_NAME "JoeRender" )
else()
    add_executable( joerender ${joerender_sources} )
endif()

target_link_libraries( joerender ${JOERENDER_LIBS} )

#-------------------------------------------------------------------------------
# copy the effect files
#-------------------------------------------------------------------------------

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/data/clear_blue.jfx
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/data/clear_blue.jfx ${CMAKE_CURRENT_BINARY_DIR}/data/clear_blue.jfx
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/data/clear_blue.jfx )

add_custom_target( joerender_copy_files DEPENDS "${PROJECT_BINARY_DIR}/data/clear_blue.jfx" )
add_dependencies( joerender joerender_copy_files )

