#ifndef ENJACL_CL_H_INCLUDED
#define ENJACL_CL_H_INCLUDED

#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>
#include "util.h"

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

    class ENJACL_EXPORT CL
    {
    public:
        CL();
        CL(bool with_gl);
        /*
            std::vector<Buffer> buffers;
            std::vector<Program> programs;
            std::vector<Kernel> kernels;
        
            int addBuffer(Buffer buff);
            int addProgram(Program prog);
            int addKernel(Kernel kern);
        */

        //setup an OpenCL context that shares with OpenGL
        void setup_gl_cl();
        void setup_cl();

        cl::Program loadProgram(std::string path, std::string options="");
        cl::Kernel loadKernel(std::string path, std::string name);
        cl::Kernel loadKernel(cl::Program program, std::string kernel_name);

        //TODO make more general
        void addIncludeDir(std::string);

        //TODO add oclErrorString to the class
        //move from util.h/cpp
        //

    private:
        std::string inc_dir;

        std::vector<cl::Context> context;
        std::vector<cl::CommandQueue> queue;
	//TODO maybe we should split the devices into gpu_devices and cpu_devices
        std::vector<cl::Device> devices;
        //int deviceUsed;

        //error checking stuff
        int err;
        cl::Event event;
	std::vector<cl::Event> waitForEvents;

    };







}

#endif

