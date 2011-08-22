#ifndef ENJACL_BUFFER_H_INCLUDED
#define ENJACL_BUFFER_H_INCLUDED
/*
 * The Buffer class abstracts the OpenCL Buffer and BufferGL classes
 * by providing some convenience methods
 *
 * we pass in an OpenCL instance  to the constructor 
 * which manages the underlying context and queues
 */

#include <string>
#include <vector>
//#ifdef ENABLE_PROFILING
//   #include<CLProfiler.h>
//#endif

#include <CLL.h>
#ifdef WIN32
    //#if defined(enjacl_EXPORTS)
	//This needs to be handled better. For some reason the above ifdef works
    // in all the other include files except this one.

        #define ENJACL_EXPORT __declspec(dllexport)

    //#else
    //    #define ENJACL_EXPORT __declspec(dllimport)
	//#endif 

#else
    #define ENJACL_EXPORT
#endif

namespace enjacl
{
    
    template <class T>
    class ENJACL_EXPORT Buffer
    {
    public:
        Buffer(){ cl_buffer=NULL; dev=NULL; vbo_id=0; host_buff=NULL; }
        //create an OpenCL buffer from existing data
        Buffer(EnjaDevice *dev, size_t size, cl_mem_flags memtype = CL_MEM_READ_WRITE);
        //Buffer(EnjaDevice *dev, std::vector<T>* data);
        Buffer(EnjaDevice *dev, std::vector<T>* data, cl_mem_flags memtype = CL_MEM_READ_WRITE);
        //create a OpenCL BufferGL from a vbo_id
        Buffer(EnjaDevice *dev, GLuint vbo_id, cl_mem_flags memtype = CL_MEM_READ_WRITE);
        ~Buffer();

        cl_mem getDevicePtr() { return cl_buffer(); }
        cl::Buffer& getBuffer() {return *cl_buffer;}
       
        //need to acquire and release arrays from OpenGL context if we have a VBO
        void acquire();
        void release();

        void copyToDevice(bool blocking=false);
        //pastes the data over the current array starting at [start]
        void copyToDevice(int start, bool blocking=false);

        void copyToHost(int num=0, bool blocking=false);
        void copyToHost(int start, int num=0, bool blocking=false);
        
        void copyFromBuffer(Buffer<T>& src, size_t start_src, size_t start_dst, size_t size,bool blocking=false);
        
        void create(int size, T val);
        void attachHostBuffer(std::vector<T>* buf);
        std::vector<T>* releaseHostBuffer();
        std::vector<T>* const getHostBuffer();

        cl::Event& getEvent() {
            return event;
        }

    private:
         //we will want to access buffers by name when going across systems
        //std::string name;
        //the actual buffer handled by the Khronos OpenCL c++ header
        cl::Buffer* cl_buffer;
        //std::vector<cl::Memory> cl_buffer;
        std::vector<T>* host_buff;
        EnjaDevice* dev;

        //CL *cli;
        cl::Event event;
        //std::vector<cl::Event>* waitForEvents;

        //if this is a VBO we store its id
        GLuint vbo_id;
#ifdef ENABLE_PROFILING
//
//        CLProfiler profiler;
//    public:
//        const CLProfiler& getProfiler()
//        {
//            return profiler;
//        }
#endif
    };

    #include "Buffer.cpp"

}
#endif

