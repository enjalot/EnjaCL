#ifndef RTPS_KERNEL_H_INCLUDED
#define RTPS_KERNEL_H_INCLUDED
/*
 * The Kernel class abstracts the OpenCL Kernel class
 * by providing some convenience methods
 *
 * For now we build one program per kernel. In the future
 * we will make it possible to make several kernels per program
 *
 * we pass in an OpenCL instance  to the constructor 
 * which manages the underlying context and queues
 */

#include <string>
#include <stdio.h>

#include "CLL.h"
#ifdef WIN32
    #if defined(enjacl_EXPORTS)
        #define RTPS_EXPORT __declspec(dllexport)
    #else
        #define RTPS_EXPORT __declspec(dllimport)
	#endif 
#else
    #define RTPS_EXPORT
#endif

namespace enjacl
{


    class RTPS_EXPORT Kernel
    {
    public:
        Kernel()
        {
            cli = NULL;
        };
        Kernel(CL *cli, std::string source, std::string name);
        Kernel(CL *cli, cl::Program program, std::string name);

        //we will want to access buffers by name when going accross systems
        std::string name;
        std::string source;

        CL *cli;
        //we need to build a program to have a kernel
        cl::Program program;

        //the actual OpenCL kernel object
        cl::Kernel kernel;

        template <class T> void setArg(int arg, T val);
        void setArgShared(int arg, int nb_bytes);

        void setBlocking(bool blocking) { this->blocking = blocking; };

        //execute the kernel and return the time it took in milliseconds using GPU timer
        //assumes null range for worksize offset and local worksize
        //float execute(int ndrange);
        //later we will make more execute routines to give more options
        float execute(int ndrange, int workgroup_size=0, cl::Event* event=NULL);
    private:
        bool blocking;

    };

    template <class T> void Kernel::setArg(int arg, T val)
    {
        try
        {
            kernel.setArg(arg, val);
        }
        catch (cl::Error er)
        {
            printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
        }

    }



}

#endif

