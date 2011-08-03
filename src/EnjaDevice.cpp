#include <EnjaDevice.h>

namespace enjacl
{
    cl::Program EnjaDevice::loadProgram(std::string path, std::string options, EnjaDevice* dev)
    {
        // Program Setup

        int length;
        char* src = file_contents(path.c_str(), &length);
        std::string s(src);
        free(src);
        return loadProgramFromStr(s,options,dev);
    }
    
    cl::Program EnjaDevice::loadProgramFromStr(std::string kernel_source, std::string options, EnjaDevice* dev)
    {
        
        debugf("kernel size: %d", kernel_source.size());
        debugf("kernel: %s", kernel_source.c_str());
        cl::Program program;
        try
        {
            cl::Program::Sources source(1,
                                        std::make_pair(kernel_source.c_str(), kernel_source.size()));
            program = cl::Program(dev->getContext(), source);

        }
        catch (cl::Error er)
        {
            printf("loadProgram\n");
            printf("ERROR: %s(%s)\n", er.what(), CL::oclErrorString(er.err()));
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

            debugf("OPTIONS: %s", options.c_str());
            
            std::vector<cl::Device> devices;
            devices.push_back(dev);
            program.build(devices, options.c_str());
#endif
        }
        catch (cl::Error er)
        {
            printf("loadProgram::program.build\n");
            printf("source= %s\n", kernel_source.c_str());
            printf("ERROR: %s(%s)\n", er.what(), CL::oclErrorString(er.err()));
        }
        //NOTE: Maybe this should be made into a debugf. -ASY 07/25/2011
        std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(dev) << std::endl;
        std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(dev) << std::endl;
        std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(dev) << std::endl;
        return program;
    }

    //----------------------------------------------------------------------
    cl::Kernel EnjaDevice::loadKernel(std::string path, std::string kernel_name, int context)
    {
        cl::Program program;
        cl::Kernel kernel;
        try
        {
            program = loadProgram(path, "", context);
            kernel = cl::Kernel(program, kernel_name.c_str(), NULL);
        }
        catch (cl::Error er)
        {
            printf("ERROR: %s(%s)\n", er.what(), CL::oclErrorString(er.err()));
        }
        return kernel;
    }

    cl::Kernel EnjaDevice::loadKernelFromStr(std::string kernel_source, std::string kernel_name, int context)
    {
        cl::Program program;
        cl::Kernel kernel;
        try
        {
            program = loadProgramFromStr(kernel_source, "", context);
            kernel = cl::Kernel(program, kernel_name.c_str(), NULL);
        }
        catch (cl::Error er)
        {
            printf("ERROR: %s(%s)\n", er.what(), CL::oclErrorString(er.err()));
        }
        return kernel;
    }
        
    //----------------------------------------------------------------------
    cl::Kernel EnjaDevice::loadKernel(cl::Program program, std::string kernel_name)
    {
        cl::Kernel kernel;
        try
        {
            kernel = cl::Kernel(program, kernel_name.c_str(), NULL);
        }
        catch (cl::Error er)
        {
            printf("ERROR: %s(%s)\n", er.what(), CL::oclErrorString(er.err()));
        }
        return kernel;
    }
}