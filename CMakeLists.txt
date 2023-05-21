cmake_minimum_required( VERSION 3.1 )

project( fractal-glenator )

set( OpenGL_GL_PREFERENCE LEGACY )

find_package( OpenGL REQUIRED )

set( INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( IMGUI_INCLUDE_DIRS ${INCLUDE_DIRS}/imgui )

include_directories( ${OPENGL_INCLUDE_DIRS} ${INCLUDE_DIRS} ${IMGUI_INCLUDE_DIRS} )

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory( include/glfw )

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

set( IMGUI
    src/imgui_impl_glfw.cpp
    src/imgui_impl_opengl3.cpp 
    src/imgui_draw.cpp
    src/imgui_tables.cpp
    src/imgui_widgets.cpp
    src/imgui.cpp 
    src/imgui_demo.cpp )

set( GLAD src/glad.c )

set( SRC
    src/main.cpp
    src/shader.cpp )

set( IMG src/stb_image.cpp )
     
add_executable( fractal-glenator ${GLAD} ${SRC} ${IMG} ${IMGUI} )
target_link_libraries( fractal-glenator ${OPENGL_LIBRARIES} GL dl glfw )
if( MSVC )
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0") 
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'fractal-glenator' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT fractal-glenator)
    endif()
endif()
