IF (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	MESSAGE(FATAL_ERROR "CMake generation must always be out-of-source!
    Remove the CMakeCache.txt file and try again from another folder")
ENDIF (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(test CXX)

#change this later
SET( BASE_DIR ${enjacl_SOURCE_DIR}/../../src)
SET( RTPS_DIR ${simple_SOURCE_DIR}/../rtpslib)

message("base dir: ${BASE_DIR}")

#need to include FindOPENCL.cmake to
SET( CMAKE_MODULE_PATH
     ${BASE_DIR}/cmake
     )

message("${CMAKE_MODULE_PATH}\n")


FIND_PACKAGE (GLUT)
FIND_PACKAGE (OpenGL)
FIND_PACKAGE (OPENCL)
FIND_PACKAGE (GLEW)

set (simple_INCLUDES
    ${RTPS_DIR}
    ${GLUT_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_PATH}
    ${BASE_DIR}/opencl11
)


#library
set (simple_SOURCES
    main.cpp
    )

set (simple_simple_SOURCES
    main_simple.cpp
    )


ADD_DEFINITIONS(-g )

INCLUDE_DIRECTORIES(${simple_INCLUDES})
ADD_EXECUTABLE(simple.x ${simple_SOURCES})

message("current build dir:\n")
message("${BASE_DIR}/rtps/build")

LINK_DIRECTORIES(${rtpsuite_BINARY_DIR}/rtpslib)
#${rtpsuite_BINARY_DIR}/bin)

SET(LINK_LIBS
    #${BASE_DIR}/rtps/build/rtpslib/librtps.so
    rtps
    ${GLUT_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${OPENCL_LIBRARIES}
    ${GLEW_LIBRARY}
    # /opt/local/lib/libGLEW.a #for staticl linking when distributing. should do this more cmake way
    # /usr/lib/libGLEW.a #for static linking when distributing on linux. should do this more cmake way
)
message(RTPS_EXPORTS "${rtps_EXPORTS}")


TARGET_LINK_LIBRARIES (simple.x
    ${LINK_LIBS}
)

