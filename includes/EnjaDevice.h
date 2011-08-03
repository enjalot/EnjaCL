/* 
 * File:   EnjaQueue.h
 * Author: asy10
 *
 * Created on July 26, 2011, 6:57 AM
 */

#ifndef ENJAQUEUE_H
#define	ENJAQUEUE_H

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

namespace enjacl
{
    class EnjaDevice
    {
    public:

        EnjaDevice(cl::CommandQueue q, cl::Device d, cl::Context c)
        {
            queue = q;
            dev = d;
            context = c;
        }

        EnjaDevice(const EnjaDevice& edev)
        {
            queue = edev.queue;
            dev = edev.dev;
            context = edev.context;
        }

        /*void setError(cl_int error) {
            this->error = error;
        }

        cl_int getError() const {
            return error;
        }*/

        void setContext(cl::Context context) {
            this->context = context;
        }

        cl::Context getContext() const {
            return context;
        }

        /*void setBuffers(std::map<std::string, cl::Memory> buffers) {
            this->buffers = buffers;
        }

        std::map<std::string, cl::Memory> getBuffers() const {
            return buffers;
        }

        void setKernels(std::map<std::string, cl::Kernel> kernels) {
            this->kernels = kernels;
        }

        std::map<std::string, cl::Kernel> getKernels() const {
            return kernels;
        }
        */
            //----------------------------------------------------------------------


        void setDevice(cl::Device dev) {
            this->dev = dev;
        }

        const cl::Device& getDevice() const {
            return dev;
        }

        void setQueue(cl::CommandQueue queue) {
            this->queue = queue;
        }

        cl::CommandQueue getQueue() const {
            return queue;
        }

        
        cl::Program loadProgram(std::string path, std::string options = "");
        cl::Program loadProgramFromStr(std::string kernel_source, std::string options = "");
        cl::Kernel loadKernel(std::string path, std::string kernel_name, std::string options = "");
        cl::Kernel loadKernelFromStr(std::string kernel_source, std::string kernel_name, std::string options = "");
        cl::Kernel loadKernel(cl::Program program, std::string kernel_name);
/*
        void addKernel(std::string name, Kernel k)
        {
            kernels[name]=k;   
        }
        
        void addBuffer(std::string name, Buffer b)
        {
            buffers[name]=b;
        }
        
        //void addImage(std::string name, Image i);
        
        Kernel& getKernel(std::string name)
        {
            return kernels[name];
        }
        Buffer& getBuffer(std::string name)
        {
            return buffers[name];
        }
        //Image& getImage(std::string name);
*/
    private:
        cl::CommandQueue queue;
        cl::Device dev;
        cl::Context context;
        //cl_int error;
        ///NOTE: According to OpenCL 1.1 API Documentation, kernel object's clSetKernelArgs is not thread-safe.
        ///Therefore one should have a separate kernel object per-device even for the
        ///same kernel to ensure proper execution.
        //std::map<std::string,Kernel> kernels;
        //std::map<std::string,Buffer> buffers;
        //std::map<std::string,Image> images;
    };
}

#endif	/* ENJAQUEUE_H */

