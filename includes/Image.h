#ifndef ENJACL_IMAGE_H_INCLUDED
#define ENJACL_IMAGE_H_INCLUDED
/*
 * The Image class abstracts the OpenCL Image and ImageGL classes
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
    class ENJACL_EXPORT Image
    {
    public:
        Image(){ cl_image=NULL; dev=NULL; vbo_id=0; host_buff=NULL; }
        //create an OpenCL image from existing data
        Image(EnjaDevice *dev, size_t size, cl_mem_flags memtype = CL_MEM_READ_WRITE);
        //Image(EnjaDevice *dev, std::vector<T>* data);
        Image(EnjaDevice *dev, std::vector<T>* data, cl_mem_flags memtype = CL_MEM_READ_WRITE);
        //create a OpenCL ImageGL from a vbo_id
        Image(EnjaDevice *dev, GLuint vbo_id, cl_mem_flags memtype = CL_MEM_READ_WRITE);

        //Image(const Image<T>& b);
        //Image<T>& operator=(const Image<T>& b);
        ~Image();

        cl_mem getDevicePtr() { return cl_image(); }
        cl::Image& getImage() {return *cl_image;}
       
        //need to acquire and release arrays from OpenGL context if we have a VBO
        void acquire();
        void release();

        void copyToDevice(bool blocking=false);
        //pastes the data over the current array starting at [start]
        void copyToDevice(int start, bool blocking=false);

        void copyToHost(int num=0, bool blocking=false);
        void copyToHost(int start, int num=0, bool blocking=false);
        
        void copyFromImage(Image<T>& src, size_t start_src, size_t start_dst, size_t size,bool blocking=false);
        
        void create(int size, T val);
        void attachHostImage(std::vector<T>* buf);
        std::vector<T>* releaseHostImage();
        std::vector<T>& getHostImage();

        //T& operator[](int i)
        //{
            //if(host_buff)
        //        return (*host_buff)[i];
            //debugf("%s", "host_buff has not been initialized yet. Can't access its elements.");
            //return T();
        //}

        T& get(int x, int y, int z=0)
        {
            return (*host_buff)[x+y*ysize+z*xsize*ysize];
        }

        int size()
        {
            if(host_buff)
                return host_buff->size();
            else
                return 0;
        }
        cl::Event& getEvent() {
            return event;
        }

    private:
         //we will want to access images by name when going across systems
        //std::string name;
        //the actual image handled by the Khronos OpenCL c++ header
        cl::Image* cl_image;
        //std::vector<cl::Memory> cl_image;
        std::vector<T>* host_image;
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

    #include "Image.cpp"

}
#endif

