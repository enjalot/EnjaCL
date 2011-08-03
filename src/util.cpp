#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <GL/glew.h>
#include <CL/cl.hpp>

#include "util.h"

namespace enjacl
{

    #ifdef DEBUG
    #include<stdarg.h>
    void dprint(const char* file, int line, const char* func, const char* msg, ...)
    {
        if (msg)
        {
            char buf[DEBUG_MAX_LINE_LEN + 1];
            va_list argp;
            va_start(argp, msg);
            vsnprintf(buf, DEBUG_MAX_LINE_LEN, msg, argp);
            va_end(argp);
            printf("\033[35m%s %d: \033[34m%s(): \033[33m%s\033[0m\n",file,line,func,buf);
        }
    }
    #endif
    unsigned int nlpo2(register unsigned int x)
    {
        x--;
        x |= (x >> 1); 
        x |= (x >> 2); 
        x |= (x >> 4); 
        x |= (x >> 8); 
        x |= (x >> 16);
        return(x+1);
    }


    char *file_contents(const char *filename, int *length)
    {
        FILE *f = fopen(filename, "r");
        void *buffer;

        if (!f)
        {
            fprintf(stderr, "Unable to open %s for reading\n", filename);
            return NULL;
        }

        fseek(f, 0, SEEK_END);
        *length = ftell(f);
        fseek(f, 0, SEEK_SET);

        buffer = malloc(*length+1);
        *length = fread(buffer, 1, *length, f);
        fclose(f);
        ((char*)buffer)[*length] = '\0';

        return(char*)buffer;
    }

#ifdef OPENGL
    int deleteVBO(GLuint id)
    {
        glBindBuffer(1, id);
        glDeleteBuffers(1, (GLuint*)&id);
        return 1; //success
    }

    GLuint createVBO(const void* data, int dataSize, GLenum target, GLenum usage)
    {
        GLuint id = 0;  // 0 is reserved, glGenBuffersARB() will return non-zero id if success

        glGenBuffers(1, &id);                        // create a vbo
        glBindBuffer(target, id);                    // activate vbo id to use
        glBufferData(target, dataSize, data, usage); // upload data to video card

        // check data size in VBO is same as input array, if not return 0 and delete VBO
        int bufferSize = 0;
        glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
        if (dataSize != bufferSize)
        {
            glDeleteBuffers(1, &id);
            id = 0;
            //cout << "[createVBO()] Data size is mismatch with input array\n";
            printf("[createVB()] Data size is mismatch with input array\n");
        }
        //this was important for working inside blender!
        glBindBuffer(target, 0);

        return id;      // return VBO id
    }
#endif

    const char* oclEventString(cl_int evt_stat)
    {
        switch(evt_stat)
        {
            case CL_QUEUED:
            {
                return "Queued";
                break;
            }
            case CL_SUBMITTED:
            {
                return "Submitted";
                break;
            }
            case CL_RUNNING:
            {
                return "Running";
                break;
            }
            case CL_COMPLETE:
            {
                return "Complete";
                break;
            }
            default:
            {
                return "Unknown Status";
                break;
            }
        }
    }
}
