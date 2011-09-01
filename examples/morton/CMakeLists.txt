IF (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	MESSAGE(FATAL_ERROR "CMake generation must always be out-of-source!
    Remove the CMakeCache.txt file and try again from another folder")
ENDIF (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(morton CXX)


INCLUDE_DIRECTORIES( 
    ${morton_SOURCE_DIR}
    #opencl related stuff:
    ${enjacl_SOURCE_DIR}/includes   #TODO: have a FindEnjaCL.cmake
    ${enjacl_SOURCE_DIR}/opencl11   #ideally this should come from using enjacl
)

#provide the source path so we can open our .cl file at runtime
#ADD_DEFINITIONS(-DCL_SOURCE_DIR="${morton_SOURCE_DIR}")
ADD_DEFINITIONS(-DMORTON_SOURCE_DIR="${morton_SOURCE_DIR}")
#build in debug mode
ADD_DEFINITIONS(-g )

#the source files we want to compile into the library
set (LIBCXXFILES 
        common/Hash.cpp
        common/Permute.cpp
        common/Neighbor.cpp
        common/NNS.cpp
   )
#set a CMake variable to name the library
SET(mortonlib morton)
ADD_LIBRARY(${mortonlib} ${LIBCXXFILES})


#create the executable
SET(EXEC morton.x)
ADD_EXECUTABLE(${EXEC} morton.cpp)

LINK_DIRECTORIES(${enjacl_BINARY_DIR})

TARGET_LINK_LIBRARIES (${EXEC}
   ${mortonlib}
   enjacl
)
