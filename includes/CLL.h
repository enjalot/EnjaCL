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

        cl::Program loadProgram(std::string path, std::string options="", int context=0);
        cl::Program loadProgramFromStr(std::string src, std::string options="", int context=0);
        cl::Kernel loadKernel(std::string path, std::string name, int context=0);
        cl::Kernel loadKernel(cl::Program program, std::string kernel_name);

        //TODO make more general
        void addIncludeDir(std::string);

        void setWaitForEvents(std::vector<cl::Event> waitForEvents) {
            this->waitForEvents = waitForEvents;
        }

        std::vector<cl::Event> getWaitForEvents() const {
            return waitForEvents;
        }

        void setEvent(cl::Event event) {
            this->event = event;
        }

        cl::Event getEvent() const {
            return event;
        }

        void setError(int error)
        {
            err = error;
        }
        const char* getErrorString() const 
        {
            return oclErrorString(err);
        }

        const std::vector<cl::Device>& getDevices() const {
            return devices;
        }

        const std::vector<cl::CommandQueue>& getQueues() const {
            return queues;
        }

        const std::vector<cl::Context>& getContexts() const {
            return contexts;
        }
        
        const std::string& getIncludeDirs() const {
            return inc_dir;
        }
        
        static const char* oclErrorString(int error)
        {
            static const char* errorString[] = {
            "CL_SUCCESS",
            "CL_DEVICE_NOT_FOUND",
            "CL_DEVICE_NOT_AVAILABLE",
            "CL_COMPILER_NOT_AVAILABLE",
            "CL_MEM_OBJECT_ALLOCATION_FAILURE",
            "CL_OUT_OF_RESOURCES",
            "CL_OUT_OF_HOST_MEMORY",
            "CL_PROFILING_INFO_NOT_AVAILABLE",
            "CL_MEM_COPY_OVERLAP",
            "CL_IMAGE_FORMAT_MISMATCH",
            "CL_IMAGE_FORMAT_NOT_SUPPORTED",
            "CL_BUILD_PROGRAM_FAILURE",
            "CL_MAP_FAILURE",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "CL_INVALID_VALUE",
            "CL_INVALID_DEVICE_TYPE",
            "CL_INVALID_PLATFORM",
            "CL_INVALID_DEVICE",
            "CL_INVALID_CONTEXT",
            "CL_INVALID_QUEUE_PROPERTIES",
            "CL_INVALID_COMMAND_QUEUE",
            "CL_INVALID_HOST_PTR",
            "CL_INVALID_MEM_OBJECT",
            "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
            "CL_INVALID_IMAGE_SIZE",
            "CL_INVALID_SAMPLER",
            "CL_INVALID_BINARY",
            "CL_INVALID_BUILD_OPTIONS",
            "CL_INVALID_PROGRAM",
            "CL_INVALID_PROGRAM_EXECUTABLE",
            "CL_INVALID_KERNEL_NAME",
            "CL_INVALID_KERNEL_DEFINITION",
            "CL_INVALID_KERNEL",
            "CL_INVALID_ARG_INDEX",
            "CL_INVALID_ARG_VALUE",
            "CL_INVALID_ARG_SIZE",
            "CL_INVALID_KERNEL_ARGS",
            "CL_INVALID_WORK_DIMENSION",
            "CL_INVALID_WORK_GROUP_SIZE",
            "CL_INVALID_WORK_ITEM_SIZE",
            "CL_INVALID_GLOBAL_OFFSET",
            "CL_INVALID_EVENT_WAIT_LIST",
            "CL_INVALID_EVENT",
            "CL_INVALID_OPERATION",
            "CL_INVALID_GL_OBJECT",
            "CL_INVALID_BUFFER_SIZE",
            "CL_INVALID_MIP_LEVEL",
            "CL_INVALID_GLOBAL_WORK_SIZE",
        };

        const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

        const int index = -error;

        return(index >= 0 && index < errorCount) ? errorString[index] : "";
    }

    private:
        std::string inc_dir;

        std::vector<cl::Context> contexts;
        //TODO: Maybe we should primarily expose a map<int/*DEVICE_TYPE*/, vector<EnjaQueues/*This would be a data structure which has a context/device associated with it thus simplifying queue events.*/> >
        std::vector<cl::CommandQueue> queues;
	//TODO maybe we should split the devices into gpu_devices and cpu_devices
        std::vector<cl::Device> devices;
        //int deviceUsed;
        
        //std::map<cl_device_type,EnjaQueue> dev_queues;

        //error checking stuff
        int err;
        cl::Event event;
	//TODO: Find out which class to put this in. These should be held per command queue and will be used for synchroniztion of both out-of-order execution and multi device execution.
	std::vector<cl::Event> waitForEvents;

    };







}

#endif

