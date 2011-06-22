CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
project(morton)

#need to include FindOPENCL.cmake to
SET( CMAKE_MODULE_PATH ${morton_SOURCE_DIR}/cmake)

message("CMake module path: ${CMAKE_MODULE_PATH}\n")

add_subdirectory(enjacl)

#FIND_PACKAGE (OPENCL)
#FIND_PACKAGE (VTK REQUIRED)
#INCLUDE(${VTK_USE_FILE})

INCLUDE_DIRECTORIES( 
    ${morton_SOURCE_DIR}
    #opencl related stuff:
    ${enjacl_SOURCE_DIR}/includes   #TODO: have a FindEnjaCL.cmake
    ${enjacl_SOURCE_DIR}/opencl11   #ideally this should come from using enjacl
)

message("include directories: \n")
get_directory_property(includes INCLUDE_DIRECTORIES)
message("${includes}\n")

#provide the source path so we can open our .cl file at runtime
#ADD_DEFINITIONS(-DCL_SOURCE_DIR="${morton_SOURCE_DIR}")
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
   #vtkHybrid
)
