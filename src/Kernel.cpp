#include <Kernel.h>

namespace enjacl
{

    Kernel::Kernel(CL *cli, std::string path, std::string name)
    {
        this->cli = cli;
        this->name = name;
        this->path = path;
        this->blocking = true;
        //TODO need to save the program
        kernel = cli->loadKernel(path, name);
    }
    Kernel::Kernel(CL *cli, cl::Program prog, std::string name)
    {
        this->cli = cli;
        this->name = name;
        this->program = prog;
        this->blocking = true;
        kernel = cli->loadKernel(program, name);
        //TODO need to save the program
        //kernel = cli->loadKernel(source, name);
    }
    
    Kernel::Kernel(const Kernel& k)
    {
        debugf("%s","here");
        this->cli = k.cli;
        this->name = k.name;
        this->path = k.path;
        this->program = k.program;
        this->blocking = true;
        kernel = k.kernel;
    }
        
    Kernel& Kernel::operator=(const Kernel& k)
    {
        debugf("%s","here");
        this->cli = k.cli;
        this->name = k.name;
        this->path = k.path;
        this->program = k.program;
        this->blocking = true;
        kernel = k.kernel;
        return *this;
    }

    //float Kernel::execute(int ndrange, int worksize=0, cl::Event* event=NULL)
    float Kernel::execute(int ndrange, int worksize, cl::Event* event, int queueID)
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
        bool del_event = false;
        try
        {
            //cl::Event event;
            if(event == NULL)
            {
                event = new cl::Event();
                del_event = true;
            }
            cli->setError(cli->getQueues()[queueID].enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global), local_range, NULL, event));
            if(blocking)
            {
                cli->getQueues()[queueID].finish();
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
        if(del_event)
        {
            delete event;
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
