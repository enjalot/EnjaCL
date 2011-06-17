IF (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	MESSAGE(FATAL_ERROR "CMake generation must always be out-of-source!
    Remove the CMakeCache.txt file and try again from another folder")
ENDIF (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(enjacl C CXX)

#change the cmake_module_path to wherever your .cmake files are stored
SET( CMAKE_MODULE_PATH ${enjacl_SOURCE_DIR}/cmake)
message("cmake_module_path: ${CMAKE_MODULE_PATH}\n")

#the khronos cpp headers for OpenCL from http://www.khronos.org/registry/cl/
SET( OPENCL_INCLUDE_DIR ${enjacl_SOURCE_DIR}/opencl11)
message("OPENCL INCLUDE: ${OPENCL_INCLUDE_DIR}\n")

ADD_DEFINITIONS( -g )
#ADD_DEFINITIONS(-DCL_VERSION_1_1)
#ADD_DEFINITIONS(-DCL_VERSION_1_0)
#ADD_DEFINITIONS(-DDEBUG)

FIND_PACKAGE (OPENCL)
FIND_PACKAGE (OpenGL)
FIND_PACKAGE (GLEW)

set (enjacl_INCLUDES
    #${GLUT_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_PATH}
    ${OPENCL_INCLUDE_DIR}
    ${enjacl_SOURCE_DIR}/includes
)    

set (enjacl_SOURCES
    ${enjacl_SOURCE_DIR}/src/CLL.cpp
    ${enjacl_SOURCE_DIR}/src/Kernel.cpp
    ${enjacl_SOURCE_DIR}/src/structs.cpp
    ${enjacl_SOURCE_DIR}/src/grid.cpp
    ${enjacl_SOURCE_DIR}/src/util.cpp
)



INCLUDE_DIRECTORIES(${enjacl_INCLUDES})
#ADD_LIBRARY(enjacl SHARED ${enjacl_SOURCES})
set(ENJACL_LIB enjacl)
ADD_LIBRARY(${ENJACL_LIB} STATIC ${enjacl_SOURCES})

TARGET_LINK_LIBRARIES (${ENJACL_LIB}
   ${OPENCL_LIBRARIES}
   ${OPENGL_LIBRARIES}
   ${GLEW_LIBRARY}
   #/opt/local/lib/libGLEW.a #for static linking when distributing on mac. should do this more cmake way
   #/usr/lib/libGLEW.a #for static linking when distributing on linux. should do this more cmake way
)



