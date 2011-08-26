#include <stdio.h>
#include <iostream>

//#include <GL/glew.h>
#if defined __APPLE__ || defined(MACOSX)
//OpenGL stuff
#elif defined(WIN32)
#else
//OpenGL stuff
    #include <GL/glx.h>
#endif

//

//#include <CLL.h>
#include <CLL.h>
#include <util.h>

namespace enjacl
{
    CL::CL()
    {
        setup_cl();
    }


    CL::CL(bool with_gl=false)
    {
        if(with_gl)
            setup_gl_cl();
        else
            setup_cl();
    }

    CL::CL(const CL& cl)
    {
        this->contexts = cl.contexts;
        this->dev_queues = cl.dev_queues;
        this->devices = cl.devices;
        this->event = cl.event;
        this->queues = cl.queues;
    }

    void CL::setup_cl()
    {

            std::vector<cl::Platform> platforms;
            cl::Platform::get(&platforms);
            debugf("platforms.size(): %zd", platforms.size());

            //for(int i=0; i<platforms.size(); i++)
            //{
            
                debugf("platform name: %s", platforms[0].getInfo<CL_PLATFORM_NAME>().c_str());

                //std::vector<cl::Device> tmp_dev;
                cl_device_type dev_types[2] = {CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU};
//                cl_device_type dev_types[1] = {CL_DEVICE_TYPE_GPU};
                for (int j = 0; j < sizeof(dev_types)/sizeof(cl_device_type); j++) 
                {
                    try
                    {

                        std::vector<cl::Device> tmp_dev;
                        platforms[0].getDevices(dev_types[j], &tmp_dev);
                        debugf("tmp_dev.size(): %zd", tmp_dev.size());

                        for (int k = 0; k < tmp_dev.size(); k++) 
                        {
                            std::vector<cl::Device> dev;
                            dev.push_back(tmp_dev[k]);
                            cl_context_properties properties[] ={CL_CONTEXT_PLATFORM, (cl_context_properties) (platforms[0])(), 0};
                            contexts.push_back(cl::Context(dev, properties));
                            devices.push_back(tmp_dev[k]);
                        }
                    }
                    catch(cl::Error er)
                    {
                        printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
                    }
                }
                cl_command_queue_properties cq_props = CL_QUEUE_PROFILING_ENABLE;
//                cl_context_properties properties[] ={CL_CONTEXT_PLATFORM, (cl_context_properties) (platforms[0])(), 0};
//                contexts.push_back(cl::Context(devices, properties));
                for (int j = 0; j < devices.size(); j++) {
//                    queues.push_back(cl::CommandQueue(contexts.back(), devices[j], cq_props, NULL));
                    queues.push_back(cl::CommandQueue(contexts[j], devices[j], cq_props, NULL));
                    cl_device_type type = devices[j].getInfo<CL_DEVICE_TYPE >();
                    debugf("type = %d",type);
                    dev_queues[type].push_back(EnjaDevice(queues[j], devices[j], contexts[j]));
//                    dev_queues[type].push_back(EnjaDevice(queues[j], devices[j], contexts.back()));
                }
           //}

        
    }

    void CL::setup_gl_cl()
    {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        debugf("platforms.size(): %zd", platforms.size());

        //for(int i=0; i<platforms.size(); i++)
        //{

            debugf("platform name: %s", platforms[0].getInfo<CL_PLATFORM_NAME>().c_str());

            //std::vector<cl::Device> tmp_dev;
            cl_device_type dev_types[2] = {CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU};
//          cl_device_type dev_types[1] = {CL_DEVICE_TYPE_GPU};
            for (int j = 0; j < sizeof(dev_types)/sizeof(cl_device_type); j++) {
		try
		{
                std::vector<cl::Device> tmp_dev;
                platforms[0].getDevices(dev_types[j], &tmp_dev);
//
//                if(dev_types[j]==CL_DEVICE_TYPE_GPU)
//                {
//                #if defined (__APPLE__) || defined(MACOSX)
//                    CGLContextObj kCGLContext = CGLGetCurrentContext();
//                    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
//                    cl_context_properties props[] =
//                    {
//                        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
//                        0
//                    };
//                    //NOTE: This needs to be tested on apple. I am trying to create a context with a collection of devices
//                    contexts.push_back(cl::Context(tmp_dev, props));   //had to edit line 1448 of cl.hpp to add this constructor
//                #else
//                #if defined WIN32 // Win32
//                    cl_context_properties props[] = 
//                    {
//                        CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), 
//                        CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), 
//                        CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
//                        0
//                    };
//                    contexts.push_back(cl::Context(tmp_dev, props));
//                #else
//                    cl_context_properties props[] = 
//                    {
//                        CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(), 
//                        CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(), 
//                        CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
//                        0
//                    };
//                    contexts.push_back(cl::Context(tmp_dev, props));
//                #endif
//                #endif
//                }
//                else
//                {
//                    cl_context_properties properties[] ={CL_CONTEXT_PLATFORM, (cl_context_properties) (platforms[0])(), 0};
//                    contexts.push_back(cl::Context(tmp_dev, properties));
//                }

                    debugf("tmp_dev.size(): %zd", tmp_dev.size());
                    debugf("devicetype = %d",dev_types[j]);
//                    for (int k = 0; k < tmp_dev.size(); k++) {
//                        std::vector<cl::Device> dev;
//                        dev.push_back(tmp_dev[k]);

                    if(dev_types[j]==CL_DEVICE_TYPE_GPU)
                    {
                    #if defined (__APPLE__) || defined(MACOSX)
                        CGLContextObj kCGLContext = CGLGetCurrentContext();
                        CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
                        cl_context_properties props[] =
                        {
                            CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
                            0
                        };
                        //NOTE: This needs to be tested on apple. I am trying to create a context with a collection of devices
                        contexts.push_back(cl::Context(tmp_dev, props));   //had to edit line 1448 of cl.hpp to add this constructor
                    #else
                    #if defined WIN32 // Win32
                        cl_context_properties props[] = 
                        {
                            CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), 
                            CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), 
                            CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
                            0
                        };
                        contexts.push_back(cl::Context(tmp_dev, props));
                    #else
                        cl_context_properties props[] = 
                        {
                            CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(), 
                            CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(), 
                            CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
                            0
                        };
                        contexts.push_back(cl::Context(tmp_dev, props));
                    #endif
                    #endif
                    }
                    else
                    {
                        cl_context_properties properties[] ={CL_CONTEXT_PLATFORM, (cl_context_properties) (platforms[0])(), 0};
                        contexts.push_back(cl::Context(tmp_dev, properties));
                    }
                    cl_command_queue_properties cq_props = CL_QUEUE_PROFILING_ENABLE;
                for (int k = 0; k < tmp_dev.size(); k++) {
                    //std::vector<cl::Device> dev;
                    //dev.push_back(tmp_dev[k]);
                    devices.push_back(tmp_dev[k]);
                    queues.push_back(cl::CommandQueue(contexts.back(), tmp_dev[k], cq_props, NULL));
    //                queues.push_back(cl::CommandQueue(contexts[k], devices[k], cq_props, NULL));
    //                cl_device_type type = devices[k].getInfo<CL_DEVICE_TYPE >();
    //                debugf("type = %d",type);
    //                dev_queues[type].push_back(EnjaDevice(queues[k], devices[k], contexts[k]));
                    dev_queues[dev_types[j]].push_back(EnjaDevice(queues.back(), tmp_dev[k], contexts.back()));
                }
		}
		catch(cl::Error er)
		{
		    printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
		}
	}
  //          }
//            cl_command_queue_properties cq_props = CL_QUEUE_PROFILING_ENABLE;
//          cl_context_properties properties[] ={CL_CONTEXT_PLATFORM, (cl_context_properties) (platforms[0])(), 0};
//          contexts.push_back(cl::Context(devices, properties));
//            for (int j = 0; j < devices.size(); j++) {
//              queues.push_back(cl::CommandQueue(contexts.back(), devices[j], cq_props, NULL));
//                queues.push_back(cl::CommandQueue(contexts[j], devices[j], cq_props, NULL));
//                cl_device_type type = devices[j].getInfo<CL_DEVICE_TYPE >();
//                debugf("type = %d",type);
//                dev_queues[type].push_back(EnjaDevice(queues[j], devices[j], contexts[j]));
//              dev_queues[type].push_back(EnjaDevice(queues[j], devices[j], contexts.back()));
//            }
          /*std::vector<cl::Platform> platforms;
            cl::Platform::get(&platforms);
            debugf("platforms.size(): %zd", platforms.size());

            //deviceUsed = 0;
            for(int i=0; i<platforms.size(); i++)
            {
                std::vector<cl::Device> tmp_dev;
                platforms[i].getDevices(CL_DEVICE_TYPE_GPU, &tmp_dev);
                debugf("tmp_dev.size(): %zd", tmp_dev.size());
                //const char* s = devices[0].getInfo<CL_DEVICE_EXTENSIONS>().c_str();
                //printf("extensions: \n %s \n", s);
                int t = tmp_dev[i].getInfo<CL_DEVICE_TYPE>();
                debugf("type: %d %d", t, CL_DEVICE_TYPE_GPU);
                                // Define OS-specific context properties and create the OpenCL context
                    //#if defined (__APPLE_CC__)
            #if defined (__APPLE__) || defined(MACOSX)
                    CGLContextObj kCGLContext = CGLGetCurrentContext();
                    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
                    cl_context_properties props[] =
                    {
                        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
                        0
                    };
                    //NOTE: This needs to be tested on apple. I am trying to create a context with a collection of devices
                    contexts.push_back(cl::Context(tmp_dev, props));   //had to edit line 1448 of cl.hpp to add this constructor
            #else
            #if defined WIN32 // Win32
                    cl_context_properties props[] = 
                    {
                        CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), 
                        CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), 
                        CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
                        0
                    };
                    contexts.push_back(cl::Context(tmp_dev, props));
            #else
                    cl_context_properties props[] = 
                    {
                        CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(), 
                        CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(), 
                        CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[i])(),
                        0
                    };
                    contexts.push_back(cl::Context(tmp_dev, props));
            #endif
            #endif
                for(int j=0; j<tmp_dev.size(); j++)
                {

                    devices.push_back(tmp_dev[j]);                      
                    //FIXME: assume sharing for now, at some point we should implement a check
                    //to make sure the devices can do context sharing

                    //create the command queue for each device that we will use to execute OpenCL commands
                    cl_command_queue_properties cq_props = CL_QUEUE_PROFILING_ENABLE;
                    queues.push_back(cl::CommandQueue(contexts.back(), devices.back(), cq_props, NULL));
                }
            }*/
    }
}
