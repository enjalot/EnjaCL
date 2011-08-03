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

#include <CLL.h>
#include <EnjaDevice.h>
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
            dev = NULL;
        };
        Kernel(EnjaDevice* dev, std::string source, std::string name);
        Kernel(EnjaDevice* dev, cl::Program program, std::string name);
        /**
         * Shallow copy constructor for kernel
         * @param k
         * @author Andrew S. Young
         * @date 7/25/11 1:27 PM
         */
        Kernel(const Kernel& k);
        Kernel& operator=(const Kernel& k);

        template <class T> void setArg(int arg, T val);
        void setArgShared(int arg, int nb_bytes);

        //execute the kernel and return the time it took in milliseconds using GPU timer
        //assumes null range for worksize offset and local worksize
        //float execute(int ndrange);
        //later we will make more execute routines to give more options
        float execute(int ndrange, int workgroup_size=0, bool blocking = false);

        void build(std::string options = "");
        void buildFromStr(std::string kernel_source, std::string options = "");

        void setKernel(cl::Kernel kernel) {
            this->kernel = kernel;
        }

        cl::Kernel getKernel() const {
            return kernel;
        }

        void setProgram(cl::Program program) {
            this->program = program;
        }

        cl::Program getProgram() const {
            return program;
        }

        void setEnjaDevice(EnjaDevice* dev) {
            this->dev = dev;
        }

        EnjaDevice* getEnjaDevice() const {
            return dev;
        }

        void setName(std::string name) {
            this->name = name;
        }

        std::string getName() const {
            return name;
        }
    private:
        //we will want to access buffers by name when going across systems
        std::string name;
        std::string path;

        EnjaDevice* dev; 
        //we need to build a program to have a kernel
        cl::Program program;

        //the actual OpenCL kernel object
        cl::Kernel kernel;

        cl::Event event;

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

