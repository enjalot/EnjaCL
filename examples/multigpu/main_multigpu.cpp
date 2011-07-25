
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include <time.h>
//#include <utils.h>
//#include <string.h>
//#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>

#include "CLL.h"
#include "Kernel.h"
//#include "util.h"
#include "timer_eb.h"

const int NDRANGE = 1;

using namespace std;
using namespace enjacl;
using namespace EB;

const string kernel_str = "\n__kernel void vect_add(__global float* a, __global float* b, global float* c)\n"
                           "{\n"
                           "    int index = get_global_id(0);\n"
                           "    c[index] = a[index] + b[index];\n"
                           "}\n";

//timers
//GE::Time *ts[3];

void printFloatVector(vector<float> vec)
{
    printf("[");
    for(int i = 0; i<vec.size(); i++)
    {
        printf("%f,",vec[i]);
    }
    printf("\b]\n");
}

class CLProfiler
{
public:
    void addEvent(const char* name, int device_num, int num_dev, cl::Event& event)
    {
        cl_ulong start, end, queued, submit;
        event.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
        event.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
        event.getProfilingInfo(CL_PROFILING_COMMAND_QUEUED, &queued);
        event.getProfilingInfo(CL_PROFILING_COMMAND_SUBMIT, &submit);
        double timing = (end - start) * 1.0e-6; 
        stringstream s;
        s<<name<< " # "<<device_num+1<<" of "<<num_dev;
        cl_timing& tmp = timings[s.str()]; 
        tmp.end_time = end * 1.0e-6;
        tmp.start_time = start * 1.0e-6;
        tmp.queue_time = queued * 1.0e-6;
        tmp.submit_time = submit * 1.0e-6;
        if(timing>tmp.max_time)
            tmp.max_time = timing;
        if(timing<tmp.min_time)
            tmp.min_time = timing;
        tmp.total_time += timing;
        tmp.num_times += 1;
    }
    void printAll()
    {
        for (map<string, cl_timing>::iterator i = timings.begin();
              i!=timings.end(); i++)
        {
            cout<<i->first<<":"<<endl;
            /*cout<<"\tMinimum Time:\t\t"<<*min_element(i->second.begin(),i->second.end())<<endl;
            cout<<"\tMaximum Time:\t\t"<<*max_element(i->second.begin(),i->second.end())<<endl;
            float total = 0.0;
            for(vector<float>::iterator j = i->second.begin(); j!=i->second.end(); j++)
                total+=*j;
            cout<<"\tAverage Time:\t\t"<<total/i->second.size()<<endl;
            cout<<"\tTotal Time:\t\t"<<total<<endl;
            cout<<"\tCount:\t\t"<<i->second.size()<<"\n"<<endl;**/
            cout<<setprecision(15)<<fixed;
            cout<<"\tSubmit Time:\t\t"<<i->second.submit_time<<endl;
            cout<<"\tQueue Time:\t\t"<<i->second.queue_time<<endl;
            cout<<"\tStart Time:\t\t"<<i->second.start_time<<endl;
            cout<<"\tEnd Time:\t\t"<<i->second.end_time<<endl;
            cout<<"\tMinimum Time:\t\t"<<i->second.min_time<<endl;
            cout<<"\tMaximum Time:\t\t"<<i->second.max_time<<endl;
            cout<<"\tAverage Time:\t\t"<<i->second.total_time/i->second.num_times<<endl;
            cout<<"\tTotal Time:\t\t"<<i->second.total_time<<endl;
            cout<<"\tCount:\t\t"<<i->second.num_times<<"\n"<<endl;
        }
    }
private:
    struct cl_timing
    {
        cl_timing()
        {
            min_time = numeric_limits<double>::max();
            max_time = numeric_limits<double>::min();
            total_time = start_time = end_time = queue_time = submit_time = 0.0;
            num_times = 0;
        }
        double min_time;
        double max_time;
        double total_time;
        double queue_time;
        double submit_time;
        double start_time;
        double end_time;
        int num_times;
    };
    map<string, cl_timing > timings;
};

void printEventInfo(const char* name, int device_num, cl::Event& event,map<string,vector<float> >& )
{
    
}
//================
//#include "materials_lights.h"

//----------------------------------------------------------------------
float rand_float(float mn, float mx)
{
    float r = rand() / (float) RAND_MAX;
    return mn + (mx-mn)*r;
}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
int main(int argc, char** argv)
{
    int num_runs = 10;
    int vector_size = 10000000;

    CLProfiler prof;
    //Argument 1 sets the size of vectors
    if(argc>1)
    {
        vector_size = atoi(argv[1]);
    }

    //Argument 2 sets the number of times to run
    if(argc>2)
    {
        num_runs = atoi(argv[2]);
    }

    printf("Vector size is %d\n",vector_size);
    printf("Number of times to run %d\n",num_runs);

    CL* cli = new CL();

    /*cl::Program::Sources src;
    src.push_back(pair<const char*, ::size_t>(cl_vect_add.c_str(), cl_vect_add.length()));
    vector<cl::Kernel> kernels;
    for(int i = 0; i<cli->devices.size();i++)
    {
        cl::Program prog(cli->context_vec[i],src);
        vector<cl::Device> dev;
        dev.push_back(cli->devices[i]);
        prog.build(dev);
    
        kernels.push_back(cl::Kernel(prog,"vect_add"));
    }*/
    
    cl::Program progs[cli->getContexts().size()];
    Kernel kerns[cli->getContexts().size()];
    for(int i=0; i<cli->getContexts().size(); i++)
    {
        progs[i] = cli->loadProgramFromStr(kernel_str,"",i);
        kerns[i] = Kernel(cli,progs[i],"vect_add");
    }
    //Create timers to keep up with execution/memory transfer times.
    int num_timers = 3;

    EB::TimerList timers;
    timers["vect_add_cpu"] = new EB::Timer("Adding vectors on single CPU", 0);
    const char* timer_name_temp[] = {"buffer_write_%d_%ss","vect_add_%d_%ss","buffer_read_%d_%ss"};
    const char* timer_desc_temp[] = {"Writing buffers to %d %ss","Adding vectors on %d %ss","Reading buffers from %d %ss"};

    char timer_name[num_timers*cli->getDevices().size()][256];
    for(int num_gpus = 1; num_gpus<=cli->getDevices().size(); num_gpus++)
    {
        for(int i = 0;i<num_timers; i++)
        {
            char timer_desc[256];
            sprintf(timer_name[i+(num_gpus-1)*num_timers],timer_name_temp[i],num_gpus,"GPU");
            sprintf(timer_desc,timer_desc_temp[i],num_gpus,"GPU");
            timers[timer_name[i+(num_gpus-1)*num_timers]] = new EB::Timer(timer_desc,0);
        }
    }

    //create CPU Buffers of size vector_size
    vector<float> a_h(vector_size);
    vector<float> b_h(vector_size);
    vector<float> c_h(vector_size);

    //Initialize the vectors.
    for(int i = 0; i<vector_size; i++)
    {
        a_h[i]=b_h[i]=i;c_h[i]=0.0f;
    }



    //run simulation num_runs times to make sure we get a good statistical sampleing of run times.
    for(int j = 0; j<num_runs; j++)
    {
        int i;
        cout<<"run: "<<j+1<<" of "<< num_runs<<endl;
        
        //Run on 1 gpu, 2 gpus, ..., n gpus where n is the number of devices belonging to the cl context.
        for(int num_gpus = 1; num_gpus<=cli->getDevices().size(); num_gpus++)
        {
            int timer_num = 3*(num_gpus-1);
            cl::Event event_a[num_gpus],event_b[num_gpus],event_execute[num_gpus],event_read[num_gpus];

            //Create Buffers for each gpu.
            cl::Buffer a_d[num_gpus];
            cl::Buffer b_d[num_gpus];
            cl::Buffer c_d[num_gpus];

            //Set size and buffer properties for each of the buffer. Divide by num_gpus to evenly distribute
            //data accross them
            #pragma omp parallel for private(i) schedule(static,1)
            for(i = 0; i<num_gpus; i++)
            {
                a_d[i] = cl::Buffer(cli->getContexts()[0],CL_MEM_READ_ONLY,(a_h.size()/num_gpus)*sizeof(float));
                b_d[i] = cl::Buffer(cli->getContexts()[0],CL_MEM_READ_ONLY,(b_h.size()/num_gpus)*sizeof(float));
                c_d[i] = cl::Buffer(cli->getContexts()[0],CL_MEM_WRITE_ONLY,(c_h.size()/num_gpus)*sizeof(float));
            }
    
            //Transfer our host buffers to each GPU then wait for it to finish before executing the kernel.
            timers[timer_name[timer_num]]->start();
            #pragma omp parallel for private(i) schedule(static,1)
            for(i = 0; i<num_gpus; i++)
            {
                cli->setError(cli->getQueues()[i].enqueueWriteBuffer(a_d[i], CL_FALSE, 0, (a_h.size()/num_gpus)*sizeof(float), &a_h[i*(a_h.size()/num_gpus)], NULL, &event_a[i]));
                cli->setError(cli->getQueues()[i].enqueueWriteBuffer(b_d[i], CL_FALSE, 0, (b_h.size()/num_gpus)*sizeof(float), &b_h[i*(b_h.size()/num_gpus)], NULL, &event_b[i]));
		cli->getQueues()[i].flush();
                cli->getQueues()[i].finish();
            }
            timers[timer_name[timer_num]]->stop();

            //set the kernel arguments
            for(i=0;i<cli->getContexts().size(); i++)
            {
                kerns[i].getKernel().setArg(0,a_d[i]);
                kerns[i].getKernel().setArg(1,b_d[i]);
                kerns[i].getKernel().setArg(2,c_d[i]);
            }

            //Set the kernel arguments vec a,b,c and enqueue kernel.
            timers[timer_name[timer_num+1]]->start();
            #pragma omp parallel for private(i), schedule(static,1)
            for(i = 0; i<num_gpus; i++)
            {
                try
                {
                    //FIXME: Need to handle context better/kernels better.
                    cli->setError(cli->getQueues()[i].enqueueNDRangeKernel(kerns[0].getKernel(),cl::NullRange,  cl::NDRange(vector_size/num_gpus),cl::NullRange , NULL, &event_execute[i]));
		    cli->getQueues()[i].flush();
                    cli->getQueues()[i].finish();
                }
                catch (cl::Error er)
                {
                    printf("j = %d, num_gpus = %d, i = %d\n",j,num_gpus,i);
                    printf("ERROR: %s(%s)\n", er.what(), CL::oclErrorString(er.err()));
                }
            }
            timers[timer_name[timer_num+1]]->stop();
        
            timers[timer_name[timer_num+2]]->start();
            #pragma omp parallel for private(i) schedule(static,1)
            for(i = 0; i<num_gpus; i++)
            {
                cl::Event event;
                cli->setError(cli->getQueues()[i].enqueueReadBuffer(c_d[i], CL_FALSE, 0, (c_h.size()/num_gpus)*sizeof(float), &c_h[i*(c_h.size()/num_gpus)], NULL, &event_read[i]));
		cli->getQueues()[i].flush();
		cli->getQueues()[i].finish();
            }
            timers[timer_name[timer_num+2]]->stop();
    
            //add event timings from openCL to our profiler.
            for(i = 0; i<num_gpus; i++)
            {
                prof.addEvent("GPU write buffer a. GPU ",i,num_gpus,event_a[i]);
                prof.addEvent("GPU write buffer b. GPU ",i,num_gpus,event_b[i]);
                prof.addEvent("GPU execute vector add. GPU ",i,num_gpus,event_execute[i]);
                prof.addEvent("GPU read buffer. GPU ",i,num_gpus,event_read[i]);
            }
        }

        timers["vect_add_cpu"]->start();
        //#pragma omp parallel for private(i)
        for(i = 0; i<a_h.size(); i++)
        {
            c_h[i]=a_h[i]+b_h[i];
        }
        timers["vect_add_cpu"]->stop();
    }

    //cl::Program progcpu(cli->cpu_context,src);
    //progcpu.build(cli->cpu_devices);
    /*std::cout << "Build Status: " << prog.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(cli->cpu_devices.front()) << std::endl;
    std::cout << "Build Options:\t" << prog.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(cli->cpu_devices.front()) << std::endl;
    std::cout << "Build Log:\t " << prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>(cli->cpu_devices.front()) << std::endl;*/
    //cl::Kernel kernelcpu(progcpu,"vect_add");
    /*
    cl::Event event;
    for(int NUM_CPUS = 1; NUM_CPUS<=cli->cpu_devices.size(); NUM_CPUS++)
    {
        char timer_name[num_timers][256];
        for(int i = 0;i<num_timers; i++)
        {
            char timer_desc[256];
            sprintf(timer_name[i],timer_name_temp[i],NUM_CPUS,"CPU");
            sprintf(timer_desc,timer_desc_temp[i],NUM_CPUS,"CPU");
            timers[timer_name[i]] = new EB::Timer(timer_desc,0);
        }
        try
        {
            cl::Buffer a_d[NUM_CPUS];
            cl::Buffer b_d[NUM_CPUS];
            cl::Buffer c_d[NUM_CPUS];
            for(int i = 0; i<NUM_CPUS; i++)
            {
                a_d[i] = cl::Buffer(cli->cpu_context,CL_MEM_READ_ONLY,(a_h.size()/NUM_CPUS)*sizeof(float));
                b_d[i] = cl::Buffer(cli->cpu_context,CL_MEM_READ_ONLY,(b_h.size()/NUM_CPUS)*sizeof(float));
                c_d[i] = cl::Buffer(cli->cpu_context,CL_MEM_WRITE_ONLY,(c_h.size()/NUM_CPUS)*sizeof(float));
            }
    
            //printFloatVector(a_h);
            //printFloatVector(b_h);
            cl::Event event;
            timers[timer_name[0]]->start();
            for(int i = 0; i<NUM_CPUS; i++)
            {
                cli->err = cli->cpu_queue[i].enqueueWriteBuffer(a_d[i], CL_FALSE, 0, (a_h.size()/NUM_CPUS)*sizeof(float), &a_h[i*(a_h.size()/NUM_CPUS)], NULL, &event);
                cli->err = cli->cpu_queue[i].enqueueWriteBuffer(b_d[i], CL_FALSE, 0, (b_h.size()/NUM_CPUS)*sizeof(float), &b_h[i*(b_h.size()/NUM_CPUS)], NULL, &event);
            }
            for(int i = 0; i<NUM_CPUS; i++)
            {
                cli->cpu_queue[i].finish();
            }
            timers[timer_name[0]]->stop();
    
            timers[timer_name[1]]->start();
            for(int i = 0; i<NUM_CPUS; i++)
            {
                kernelcpu.setArg(0,a_d[i]);
                kernelcpu.setArg(1,b_d[i]);
                kernelcpu.setArg(2,c_d[i]);
                try
                {
                    cli->err = cli->cpu_queue[i].enqueueNDRangeKernel(kernelcpu,cl::NullRange,  cl::NDRange(vector_size/NUM_CPUS), cl::NullRange, NULL, &event);
                }
                catch (cl::Error er)
                {
                    //printf("err: work group size: %d\n", work_group_size);
                    printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
                }
            }
            for(int i = 0; i<NUM_CPUS; i++)
            {
                cli->cpu_queue[i].finish();
            }
            timers[timer_name[1]]->stop();
        
            timers[timer_name[2]]->start();
            for(int i = 0; i<NUM_CPUS; i++)
            {
                cli->err = cli->cpu_queue[i].enqueueReadBuffer(c_d[i], CL_FALSE, 0, (c_h.size()/NUM_CPUS)*sizeof(float), &c_h[i*(c_h.size()/NUM_CPUS)], NULL, &event);
            }
            for(int i = 0; i<NUM_CPUS; i++)
            {
                cli->cpu_queue[i].finish();
            }
            timers[timer_name[2]]->stop();
    
            //printFloatVector(c_h);
            //initialize the OpenGL scene for rendering
        }
        catch (cl::Error er)
        {
            printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
        }
    }
    */


    timers.printAll();
    prof.printAll();

    delete cli;
    return 0;
}