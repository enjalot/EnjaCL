CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
#project(multigpu)

INCLUDE_DIRECTORIES( 
    ${multigpu_SOURCE_DIR}
)

message("Building multigpu example: \n")

FIND_PACKAGE (GLUT)

SET(SRC timer_eb.cpp)
#create the executable
ADD_EXECUTABLE(multigpu_vec.x main_multigpu.cpp ${SRC})
#ADD_EXECUTABLE(multigpu_image.x main_multigpu_imageproc.cpp stb_image.c ${SRC})
ADD_EXECUTABLE(multigpu_gl.x main_multigpu_gl.cpp ${SRC})

LINK_DIRECTORIES(${enjacl_BINARY_DIR})

TARGET_LINK_LIBRARIES (multigpu_vec.x
   enjacl
)

#TARGET_LINK_LIBRARIES (multigpu_image.x
#   enjacl
#)

INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})

TARGET_LINK_LIBRARIES (multigpu_gl.x
   ${GLUT_LIBRARIES}
   enjacl
)
