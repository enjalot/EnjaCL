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

#include "CLL.h"
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
        Buffer(){ dev=NULL; vbo_id=0; host_buff=NULL; }
        //create an OpenCL buffer from existing data
        Buffer(EnjaDevice *dev, int size);
        Buffer(EnjaDevice *dev, std::vector<T>* data);
        Buffer(EnjaDevice *dev, std::vector<T>* data, unsigned int memtype);
        //create a OpenCL BufferGL from a vbo_id
        Buffer(EnjaDevice *dev, GLuint vbo_id, int size);
        ~Buffer();

        cl_mem getDevicePtr() { return cl_buffer(); }
        cl::Memory& getBuffer() {return cl_buffer;}
       
        //need to acquire and release arrays from OpenGL context if we have a VBO
        void acquire();
        void release();

        void copyToDevice(bool blocking=false);
        //pastes the data over the current array starting at [start]
        void copyToDevice(int start, bool blocking=false);

        //really these should take in a presized vector<T> to be filled
        //these should be factored out
        std::vector<T> copyToHost(int num, bool blocking=false);
        std::vector<T> copyToHost(int num, int start, bool blocking=false);
        //correct way (matches copyToDevice
        //void copyToHost(std::vector<T> &data, bool blocking=false);
        //void copyToHost(std::vector<T> &data, int start, bool blocking=false);


        void copyFromBuffer(Buffer<T> dst, size_t start_src, size_t start_dst, size_t size,bool blocking=false);
        
        void create(int size, T val);
        void attachHostBuffer(std::vector<T>* buf);
        std::vector<T>* releaseHostBuffer();
        std::vector<T>* const getHostBuffer();

        cl::Event& getEvent() const {
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

        //if this is a VBO we store its id
        GLuint vbo_id;
    };

    #include "Buffer.cpp"

}
#endif

