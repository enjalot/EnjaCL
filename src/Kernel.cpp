#include <Kernel.h>

namespace enjacl
{

    Kernel::Kernel(CL *cli, std::string source, std::string name)
    {
        this->cli = cli;
        this->name = name;
        this->source = source;
        this->blocking = true;
        //TODO need to save the program
        kernel = cli->loadKernel(source, name);
    }
    Kernel::Kernel(CL *cli, cl::Program prog, std::string name)
    {
        this->cli = cli;
        this->name = name;
        //this->source = source;
        this->program = prog;
        this->blocking = true;
        kernel = cli->loadKernel(program, name);
        //TODO need to save the program
        //kernel = cli->loadKernel(source, name);
    }

    //float Kernel::execute(int ndrange, int worksize=0, cl::Event* event=NULL)
    float Kernel::execute(int ndrange, int worksize, cl::Event* event)
    {
        int global;
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

        //printf("global %d, local %d\n", global, worksize);
        if (ndrange <=0)// || worksize <= 0)
            return - 1.f;

        cl::NDRange local_range;
        if(worksize <= 0)
        {
            local_range = cl::NDRange(worksize);
        }
        else
        {
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
            cli->err = cli->queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global), cl::NDRange(worksize), NULL, event);
            if(blocking)
            {
                cli->queue.finish();
                event->getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
                event->getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
                timing = (end - start) * 1.0e-6f;
            }
        }
        catch (cl::Error er)
        {
            printf("err: global %d, local %d\n", global, worksize);
            printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
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
            cli->queue.finish();
        }
        catch (cl::Error er)
        {
            printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
        }
    }

}
