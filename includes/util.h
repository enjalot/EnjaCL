//from http://www.songho.ca/opengl/gl_vbo.html

#ifndef ENJACL_UTIL_H_INCLUDED
#define ENJACL_UTIL_H_INCLUDED

#include <vector>

#ifdef WIN32
    #if defined(enjacl_EXPORTS)
        #define ENJACL_EXPORT __declspec(dllexport)
    #else
        #define ENJACL_EXPORT __declspec(dllimport)
	#endif 
#else
    #define ENJACL_EXPORT
#endif

namespace enjacl
{

    unsigned int nlpo2(register unsigned int x); 
    char ENJACL_EXPORT *file_contents(const char *filename, int *length);

#ifdef OPENGL
    GLuint ENJACL_EXPORT createVBO(const void* data, int dataSize, GLenum target, GLenum usage);
    int ENJACL_EXPORT deleteVBO(GLuint id);
#endif

    const char* ENJACL_EXPORT oclEventString(cl_int event_status);

    //NVIDIA helper functions    
    ENJACL_EXPORT const char* oclErrorString(cl_int error);
    ENJACL_EXPORT cl_int oclGetPlatformID(cl_platform_id* clSelectedPlatformID);


}

#endif
