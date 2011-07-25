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
                return "queued";
                break;
            }
            case CL_SUBMITTED:
            {
                return "submitted";
                break;
            }
            case CL_RUNNING:
            {
                return "running";
                break;
            }
            case CL_COMPLETE:
            {
                return "complete";
                break;
            }
            default:
            {
                return "dunno";
                break;
            }
        }
    }


    // Helper function to get error string
    // From NVIDIA
    // *********************************************************************




    //NVIDIA's code
    //////////////////////////////////////////////////////////////////////////////
    //! Gets the platform ID for NVIDIA if available, otherwise default to platform 0
    //!
    //! @return the id 
    //! @param clSelectedPlatformID         OpenCL platform ID
    //////////////////////////////////////////////////////////////////////////////
    /*Not needed. -ASY 07/24/2011
    cl_int oclGetPlatformID(cl_platform_id* clSelectedPlatformID)
    {
        char chBuffer[1024];
        cl_uint num_platforms;
        cl_platform_id* clPlatformIDs;
        cl_int ciErrNum;
        *clSelectedPlatformID = NULL;
        cl_uint i = 0;

        // Get OpenCL platform count
        ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);
        if (ciErrNum != CL_SUCCESS)
        {
            //shrLog(" Error %i in clGetPlatformIDs Call !!!\n\n", ciErrNum);
            printf(" Error %i in clGetPlatformIDs Call !!!\n\n", ciErrNum);
            return -1000;
        }
        else
        {
            if (num_platforms == 0)
            {
                //shrLog("No OpenCL platform found!\n\n");
                printf("No OpenCL platform found!\n\n");
                return -2000;
            }
            else
            {
                // if there's a platform or more, make space for ID's
                if ((clPlatformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id))) == NULL)
                {
                    //shrLog("Failed to allocate memory for cl_platform ID's!\n\n");
                    printf("Failed to allocate memory for cl_platform ID's!\n\n");
                    return -3000;
                }

                // get platform info for each platform and trap the NVIDIA platform if found
                ciErrNum = clGetPlatformIDs (num_platforms, clPlatformIDs, NULL);
                for (i = 0; i < num_platforms; ++i)
                {
                    ciErrNum = clGetPlatformInfo (clPlatformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
                    if (ciErrNum == CL_SUCCESS)
                    {
                        if (strstr(chBuffer, "NVIDIA") != NULL)
                        {
                            *clSelectedPlatformID = clPlatformIDs[i];
                            break;
                        }
                    }
                }

                // default to zeroeth platform if NVIDIA not found
                if (*clSelectedPlatformID == NULL)
                {
                    //shrLog("WARNING: NVIDIA OpenCL platform not found - defaulting to first platform!\n\n");
                    printf("WARNING: NVIDIA OpenCL platform not found - defaulting to first platform!\n\n");
                    *clSelectedPlatformID = clPlatformIDs[0];
                }

                free(clPlatformIDs);
            }
        }

        return CL_SUCCESS;
    }*/




}
