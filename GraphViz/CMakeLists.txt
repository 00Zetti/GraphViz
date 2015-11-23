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

#glut stuff


aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST}

	#SOURCE
	src/compound.cpp
	src/graphviz.cpp
        src/renderer.cpp

	#INCLUDE
	include/compound.h
        include/renderer.h

        #SHADER
        shader/vertexShader.glsl
        shader/fragmentShader.glsl
)

target_link_libraries(GraphViz ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES})
