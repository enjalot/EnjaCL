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
    #ifndef debugf
        #ifdef DEBUG
        #ifndef DEBUG_MAX_LINE_LEN
        #define DEBUG_MAX_LINE_LEN 1024
        #endif
            void dprint(const char* file, int line, const char* func, const char* msg, ...);

        //    void dprint(const char* file, const char* func, int line, const char* msg)
        //    {
        //        if (msg)
        //        {
        //            printf("\033[35m%s %d: \033[34m%s(): \033[33m%s\033[0m\n",file,func,line,msg);
        //        }
        //    }
            #define debugf(format, ...) dprint(__FILE__,__LINE__,__func__,format , __VA_ARGS__);
        //    #define debugm(format) dprint(__FILE__,__LINE__,format);
        #else
            #define debugf(format, ...) ((void)0)
        //    #define debugm(format) ((void)0);
        #endif
    #endif

    unsigned int nlpo2(register unsigned int x); 
    char ENJACL_EXPORT *file_contents(const char *filename, int *length);

#ifdef OPENGL
    GLuint ENJACL_EXPORT createVBO(const void* data, int dataSize, GLenum target, GLenum usage);
    int ENJACL_EXPORT deleteVBO(GLuint id);
#endif

    const char* ENJACL_EXPORT oclEventString(cl_int event_status);
}

#endif
