project(GraphViz)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#opengl stuff
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
if(OPENGL_FOUND)
    MESSAGE("OpenGL found.")
    if(GLUT_FOUND)
        MESSAGE("Glut found.")
        if(GLEW_FOUND)
            MESSAGE("Glew found.")
            include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
        else(GLEW_FOUND)
            MESSAGE("Glew missing.")
        endif(GLEW_FOUND)
    else(GLUT_FOUND)
        MESSAGE(ERROR "Glut missing.")
    endif(GLUT_FOUND)
else (OPENGL_FOUND)
    MESSAGE(ERROR "OpenGL missing.")
endif(OPENGL_FOUND)


#set path to shader files
set(VIZ_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shader/" CACHE PATH "Path to where the shader files are.")

if(WIN32)
    string(REPLACE "/" "\\\\" VIZ_DIR ${VIZ_DIR})
endif(WIN32)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h"
)

MESSAGE("${VIZ_DIR}")


#glut stuff


aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST}

        #CONFIG
        config.h.in

	#SOURCE
	src/compound.cpp
	src/graphviz.cpp
        src/renderer.cpp

	#INCLUDE
	include/compound.h
        include/renderer.h
        include/config.h

        #SHADER
        shader/connVertexShader.vert
        shader/connFragmentShader.frag
	shader/splineVertexShader.vert
	shader/splineFragmentShader.frag
	shader/splineSortFragmentShader.frag
        shader/splineSortVertexShader.vert
	shader/nodeVertexShader.vert
	shader/nodeFragmentShader.frag
)

target_link_libraries(GraphViz ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES})
