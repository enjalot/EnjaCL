CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
#project(multigpu)

INCLUDE_DIRECTORIES( 
    ${multigpu_SOURCE_DIR}
)

message("Building multigpu example: \n")

SET(SRC timer_eb.cpp)
#create the executable
ADD_EXECUTABLE(multigpu_vec.x main_multigpu.cpp ${SRC})
#ADD_EXECUTABLE(multigpu_image.x main_multigpu_imageproc.cpp stb_image.c ${SRC})

LINK_DIRECTORIES(${enjacl_BINARY_DIR})

TARGET_LINK_LIBRARIES (multigpu_vec.x
   enjacl
)

#TARGET_LINK_LIBRARIES (multigpu_image.x
#   enjacl
#)
