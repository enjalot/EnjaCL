IF (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	MESSAGE(FATAL_ERROR "CMake generation must always be out-of-source!
    Remove the CMakeCache.txt file and try again from another folder")
ENDIF (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(simple CXX)

ADD_DEFINITIONS(-DSIMPLE_SOURCE_DIR="${simple_SOURCE_DIR}")

message("enjacl includes ${enjacl_INCLUDES}")
set (simple_INCLUDES
    ${simple_SOURCE_DIR}
    #${enjacl_INCLUDES}
    ${enjacl_SOURCE_DIR}/includes
    ${enjacl_SOURCE_DIR}/opencl11
)

set (simple_SOURCES
    simple.cpp
    )


INCLUDE_DIRECTORIES(${simple_INCLUDES})
ADD_EXECUTABLE(simple.x ${simple_SOURCES})

message("enjacl bin dir: ${enjacl_BINARY_DIR}")
LINK_DIRECTORIES(${enjacl_BINARY_DIR})
SET(LINK_LIBS
    enjacl
    #${enjacl_BINARY_DIR}/libenjacl.a
)
TARGET_LINK_LIBRARIES (simple.x
    ${LINK_LIBS}
)

