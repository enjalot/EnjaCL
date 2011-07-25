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
#include "CLL.h"
#include <util.h>

namespace enjacl
{
    CL::CL()
    {
        inc_dir = "";
        setup_cl();
    }


    CL::CL(bool with_gl=false)
    {
        inc_dir = "";
        if(with_gl)
            setup_gl_cl();
        else
            setup_cl();
    }

    void CL::addIncludeDir(std::string path)
    {
        this->inc_dir += " -I" + path;// + " -I./" + std::string(COMMON_CL_SOURCE_DIR);
    }

    //----------------------------------------------------------------------
    cl::Program CL::loadProgram(std::string path, std::string options, int context)
    {
        // Program Setup

        int length;
        char* src = file_contents(path.c_str(), &length);
        std::string s(src);
        free(src);
        return loadProgramFromStr(s,options,context);
    }
    
    cl::Program CL::loadProgramFromStr(std::string kernel_source, std::string options, int context)
    {
        
        debugf("kernel size: %d", kernel_source.size());
        debugf("kernel: %s", kernel_source.c_str());
        cl::Program program;
        try
        {
            cl::Program::Sources source(1,
                                        std::make_pair(kernel_source.c_str(), kernel_source.size()));

            program = cl::Program(contexts[context], source);

        }
        catch (cl::Error er)
        {
            printf("loadProgram\n");
            printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
        }

        try
        {
            debugf("%s","build program");
            //#ifdef DEBUG
#if 0
            srand(time(NULL));
            int rnd = rand() % 200 + 100;
            char dbgoptions[100];
            //should really check for NVIDIA platform before doing this
            sprintf(dbgoptions, "%s -cl-nv-verbose -cl-nv-maxrregcount=%d", options.c_str(), rnd);
            //sprintf(options, "-D rand=%d -D DEBUG", rnd);
            err = program.build(devices, dbgoptions);
#else

            options += this->inc_dir;
            debugf("OPTIONS: %s", options.c_str());
            

            err = program.build(devices, options.c_str());
#endif
        }
        catch (cl::Error er)
        {
            printf("loadProgram::program.build\n");
            printf("source= %s\n", kernel_source.c_str());
            printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
        }
        //NOTE: Maybe this should be made into a debugf. -ASY 07/25/2011
        for(int i = 0; i<devices.size(); i++)
        {
            std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[i]) << std::endl;
            std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[i]) << std::endl;
            std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[i]) << std::endl;
        }
        return program;
    }

    //----------------------------------------------------------------------
    cl::Kernel CL::loadKernel(std::string path, std::string kernel_name, int context)
    {
        cl::Program program;
        cl::Kernel kernel;
        try
        {
            program = loadProgram(path, "", context);
            kernel = cl::Kernel(program, kernel_name.c_str(), &err);
        }
        catch (cl::Error er)
        {
            printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
        }
        return kernel;
    }

    //----------------------------------------------------------------------
    cl::Kernel CL::loadKernel(cl::Program program, std::string kernel_name)
    {
        cl::Kernel kernel;
        try
        {
            kernel = cl::Kernel(program, kernel_name.c_str(), &err);
        }
        catch (cl::Error er)
        {
            printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
        }
        return kernel;
    }

    void CL::setup_cl()
    {

        std::vector<cl::Platform> platforms;
        err = cl::Platform::get(&platforms);
        debugf("cl::Platform::get(): %s", oclErrorString(err));
        debugf("platforms.size(): %zd", platforms.size());
        
        for(int i=0; i<platforms.size(); i++)
            {
            cl_context_properties properties[] =
            { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[i])(), 0};

            std::vector<cl::Device> tmp_dev;
            err = platforms[i].getDevices(CL_DEVICE_TYPE_GPU, &tmp_dev);
            
            //this should be made more customizable later
            contexts.push_back(cl::Context(tmp_dev, properties));
            debugf("getDevices: %s", oclErrorString(err));
            debugf("tmp_dev.size(): %zd", tmp_dev.size());
            
            for(int j = 0; j<tmp_dev.size(); j++)
            {
                devices.push_back(tmp_dev[j]);
                cl_command_queue_properties cq_props = CL_QUEUE_PROFILING_ENABLE;
                try
                {
                    queues.push_back(cl::CommandQueue(contexts.back(), devices.back(), cq_props, &err));
                }
                catch (cl::Error er)
                {
                    printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
                }
            }
        }
    }

    void CL::setup_gl_cl()
    {
        std::vector<cl::Platform> platforms;
        err = cl::Platform::get(&platforms);
        debugf("cl::Platform::get(): %s", oclErrorString(err));
        debugf("platforms.size(): %zd", platforms.size());

        //deviceUsed = 0;
        for(int i=0; i<platforms.size(); i++)
        {
            std::vector<cl::Device> tmp_dev;
            err = platforms[i].getDevices(CL_DEVICE_TYPE_GPU, &tmp_dev);
            debugf("getDevices: %s", oclErrorString(err));
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
                try
                {
                    //NOTE: This needs to be tested on apple. I am trying to create a context with a collection of devices
                    contexts.push_back(cl::Context(tmp_dev, props));   //had to edit line 1448 of cl.hpp to add this constructor
                }
                catch (cl::Error er)
                {
                    printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
                }
        #else
        #if defined WIN32 // Win32
                cl_context_properties props[] = 
                {
                    CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), 
                    CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), 
                    CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
                    0
                };
                try
                {
                    contexts.push_back(cl::Context(tmp_dev, props));
                }
                catch (cl::Error er)
                {
                    printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
                }
        #else
                cl_context_properties props[] = 
                {
                    CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(), 
                    CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(), 
                    CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[i])(),
                    0
                };
                try
                {
                    contexts.push_back(cl::Context(tmp_dev, props));
                }
                catch (cl::Error er)
                {
                    printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
                }
        #endif
        #endif
            for(int j=0; j<tmp_dev.size(); j++)
            {

                devices.push_back(tmp_dev[j]);                      
                //FIXME: assume sharing for now, at some point we should implement a check
                //to make sure the devices can do context sharing
                
                //create the command queue for each device that we will use to execute OpenCL commands
                cl_command_queue_properties cq_props = CL_QUEUE_PROFILING_ENABLE;
                try
                {
                    queues.push_back(cl::CommandQueue(contexts.back(), devices.back(), cq_props, &err));
                }
                catch (cl::Error er)
                {
                    printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
                }
            }
        }
    }





}
