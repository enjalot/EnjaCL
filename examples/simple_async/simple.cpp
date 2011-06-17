//enjaCL headers
#include "CLL.h"
#include "Buffer.h"
#include "Kernel.h"
#include "grid.h"
#include "structs.h"
#include "util.h"

//for sleeping 
#include <pthread.h>
#include <time.h>

//standard headers
#include<stdio.h>
#include <vector>
using namespace std;
using namespace enjacl;

Grid SetupGrid();
void mycallback(cl_event event, cl_int status, void* user_data)
{
    printf("Event has called back. execution must be finished!\n");
}


int main(void)
{
    printf("Hello, Simple CL\n");
    //------------------------------------------------------------------------------
    //OpenCL initialization

    CL *cli = new CL();
    string cl_path(SIMPLE_SOURCE_DIR);
    cli->addIncludeDir(cl_path);

    //instantiate advect kernel
    Kernel k_simple;
    k_simple = Kernel(cli, cl_path + "/simple.cl", "simple");
    k_simple.setBlocking(false);


    Grid grid = SetupGrid();
    grid.print();
    //initialize particles in a grid, one at the center of each grid cell
    float4 offset = .5 * grid.delta;
    vector<float4> particles = grid.plantSeeds( offset );
    int num = particles.size();
    printf("Num Particles: %d\n", num);
    //Setup the OpenCL Buffer
    Buffer<float4> cl_particles = Buffer<float4>(cli, particles);

    float dt = .1f;
    float4 v = float4(.5, .5, .5, 0.);

    int iarg = 0;
    k_simple.setArg(iarg++, cl_particles.getDevicePtr());
    k_simple.setArg(iarg++, num);
    k_simple.setArg(iarg++, dt);
    k_simple.setArg(iarg++, v);


    cl::Event exec_event;
    //exec_event.setCallback(CL_COMPLETE, mycallback);
    cli->queue.finish();

    int global_size = num;
    int local_size = 64;

    float cpu_start = clock();
    k_simple.execute(global_size, local_size, &exec_event);

    int nbc = 10; //how many particles to print
    //print first few particles before
    for(int i = 0; i < nbc; i++)
    {
        printf("original pos[%d]: %f %f %f %f\n", i, particles[i].x, particles[i].y, particles[i].z, particles[i].w);
    }

    cl_int event_status;
    for(int i = 0; i < 5; i++)
    {
        exec_event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS, &event_status);
        printf("evt status: %s\n", oclEventString(event_status));
        cli->queue.flush();//this should make sure the kernel is enqueued
        sleep(1);
    }

    float cpu_end = clock();
    float timing = (cpu_end - cpu_start) / CLOCKS_PER_SEC * 1000.0;
    printf("cpu time: %f ms\n", timing);

    printf("Before Copy to Host. About to Block\n");
    cli->queue.finish();
    printf("Blocked, about to Copy to Host\n");

    cl_ulong start, end;
    exec_event.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
    exec_event.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
    printf("the kernel took %f ms\n", (end - start) * 1.0e-6f);

    exec_event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS, &event_status);
    printf("evt status: %s\n", oclEventString(event_status));


    //copy to host is blocking by default
    cl_particles.copyToHost(particles);
    //print first few particles before
    for(int i = 0; i < nbc; i++)
    {
        printf("new pos[%d]: %f %f %f %f\n", i, particles[i].x, particles[i].y, particles[i].z, particles[i].w);
    }
}


Grid SetupGrid()
{
    float4 min = float4(-10., -10., -10., 0.);
    float4 max = float4(10., 10., 10., 0.);
    int4 res = int4(50, 50, 50, 0);
    Grid grid(min, max, res);
    return grid;
}
