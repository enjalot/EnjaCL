#ifndef ENJACL_CL_H_INCLUDED
#define ENJACL_CL_H_INCLUDED

#include <map>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include <EnjaDevice.h>
#include <util.h>

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
        CL(const CL& cl);
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

        void setEvent(cl::Event event) {
            this->event = event;
        }

        const cl::Event& getEvent() const {
            return event;
        }

        const std::vector<cl::Device>& getDevices() const {
            return devices;
        }
        
        std::vector<EnjaDevice>& getEnjaDevices(cl_device_type type = CL_DEVICE_TYPE_GPU)
        {
           return dev_queues[type]; 
        }

        const std::vector<cl::CommandQueue>& getQueues() const {
            return queues;
        }

        const std::vector<cl::Context>& getContexts() const {
            return contexts;
        }

        std::vector<EnjaDevice>& operator[](cl_device_type type)
        {
            return dev_queues[type];
        }

    private:
        std::vector<cl::Context> contexts;//TODO: Maybe we should primarily expose a map<int/*DEVICE_TYPE*/, vector<EnjaQueues/*This would be a data structure which has a context/device associated with it thus simplifying queue events.*/> >
        std::vector<cl::CommandQueue> queues;
        //TODO maybe we should split the devices into gpu_devices and cpu_devices
        std::vector<cl::Device> devices;
        //int deviceUsed;
        
        std::map<cl_device_type, std::vector<EnjaDevice> > dev_queues;

        //error checking stuff
        //int err;
        cl::Event event;
        //TODO: Find out which class to put this in. These should be held per command queue and will be used for synchronization of both out-of-order execution and multi device execution.
        //std::vector<cl::Event> waitForEvents;

    };







}

#endif

