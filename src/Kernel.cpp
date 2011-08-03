#include <Kernel.h>
#include <EnjaDevice.h>

namespace enjacl
{

    Kernel::Kernel(EnjaDevice* dev, std::string path, std::string name)
    {
        this->dev = dev;
        this->name = name;
        this->path = path;
//        this->blocking = true;
//        //TODO need to save the program
//        if(fromFile)
//            kernel = dev->loadKernel(path, name);
//        else
//            kernel = dev->loadKernelFromStr(path, name);
    }
    Kernel::Kernel(EnjaDevice* dev, cl::Program prog, std::string name)
    {
        this->dev = dev;
        this->name = name;
        this->program = prog;
//        this->blocking = true;
//        kernel = dev->loadKernel(program, name);
        //TODO need to save the program
        //kernel = cli->loadKernel(source, name);
    }
    
    Kernel::Kernel(const Kernel& k)
    {
        this->dev = k.dev;
        this->name = k.name;
        this->path = k.path;
        this->program = k.program;
//        this->blocking = true;
        kernel = k.kernel;
    }
        
    Kernel& Kernel::operator=(const Kernel& k)
    {
        this->dev = k.dev;
        this->name = k.name;
        this->path = k.path;
        this->program = k.program;
//        this->blocking = true;
        kernel = k.kernel;
        return *this;
    }

    void Kernel::build(std::string options)
    {
       kernel = dev->loadKernel(path,name,options); 
    }

    void Kernel::buildFromStr(std::string kernel_source, std::string options)
    {
       kernel = dev->loadKernelFromStr(kernel_source,name,options); 
    }

    //float Kernel::execute(int ndrange, int worksize=0, cl::Event* event=NULL)
    float Kernel::execute(int ndrange, int worksize, bool blocking)
    {
        int global;
        if (worksize <= 0)
        {
            global = ndrange;
        }
        else
        {
            float factor = (1.0f * ndrange) / worksize;
            //printf("ndrange: %d\n", ndrange);
            //printf("global f: %f\n", factor);
            if ((int)factor != factor)
            {
                factor = (int)factor;
                global = worksize*factor + worksize;
                //printf("global2: %d\n", global);
            }
            else
            {
                global = ndrange;
            }
        }

        //printf("global %d, local %d\n", global, worksize);
        if (ndrange <=0)// || worksize <= 0)
            return - 1.f;


        cl::NDRange local_range;
        if(worksize > 0)
        {
            //printf("worksize: %d\n", worksize);
            local_range = cl::NDRange(worksize);
        }
        else
        {
            //printf("worksize null: %d\n", worksize);
            local_range = cl::NullRange;
        }

        cl_ulong start, end;
        float timing = -1.0f;
        try
        {
            dev->getQueue().enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global), local_range, NULL, event);
            if(blocking)
            {
                dev->getQueue().finish();
                event->getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
                event->getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
                timing = (end - start) * 1.0e-6f;
            }
        }
        catch (cl::Error er)
        {
            printf("err: global %d, local %d\n", global, worksize);
            printf("ERROR: %s(%s)\n", er.what(), CL::oclErrorString(er.err()));
        }
        return timing;
    }

    void Kernel::setArgShared(int arg, int nb_bytes)
    {
        try
        {
            kernel.setArg(arg, nb_bytes, 0);
            //NOTE: I dont think calling finish is necessary here? -ASY 07/25/2011
            //cli->getQueues()[queueID].finish();
        }
        catch (cl::Error er)
        {
            printf("ERROR: %s(%s)\n", er.what(), CL::oclErrorString(er.err()));
        }
    }

}
