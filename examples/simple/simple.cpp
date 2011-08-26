//enjaCL headers
#include "CLL.h"
#include "Buffer.h"
#include "Kernel.h"
#include "grid.h"
#include "structs.h"
#include "util.h"

//standard headers
#include<stdio.h>
#include <vector>
using namespace std;
using namespace enjacl;

Grid SetupGrid();

int main(void)
{
    printf("Hello, Simple CL\n");
    //------------------------------------------------------------------------------
    //OpenCL initialization

    CL cli;
    string cl_path(SIMPLE_SOURCE_DIR);

    //Obtain GPU devices available on this system
    vector<EnjaDevice>& devs = cli[CL_DEVICE_TYPE_GPU];
    //choose the first one
    EnjaDevice* ed = &devs[0];

    //instantiate advect kernel
    Kernel k_simple = Kernel(ed, cl_path + "/simple.cl", "simple");
    string options("-I" + cl_path);     //tell OpenCL where to look for include files
    k_simple.build(options);
/*
    //alternatively we could create it with these functions
    Kernel k_simple;
    k_simple.setEnjaDevice(&(*devs)[0]);        //We just choose first GPU in the list
    k_simple.setName("simple");                 //our kernel is named simple
    k_simple.buildFromStr(kernel_str);          //we build the kernel from a string
*/

    //Setup a cartesian grid (regular grid)
    Grid grid = SetupGrid();
    grid.print();
    //initialize particles in a grid, one at the center of each grid cell
    float4 offset = .5 * grid.delta;
    //place a seed particle in the middle of each grid cell
    vector<float4>* particles = new vector<float4>(grid.plantSeeds( offset ));
    int num = particles->size();
    printf("Num Particles: %d\n", num);

    //Setup the OpenCL Buffer
    Buffer<float4> cl_particles(ed, particles);

    float dt = .1f;
    float4 v = float4(.5, .5, .5, 0.);

    int iarg = 0;
    k_simple.setArg(iarg++, cl_particles.getBuffer());
    k_simple.setArg(iarg++, num);
    k_simple.setArg(iarg++, dt);
    k_simple.setArg(iarg++, v);

    int global_size = num;
    int local_size = 64;
    //execute may adjust global size to meet opencl requirement that local size divides evenly
    //this is why you should always check that your thread id is less than the # of particles you want to process
    k_simple.execute(global_size, local_size);

    int nbc = 10; //how many particles to print
    //print first few particles before
    //vector<float4>& startpos = cl_particles->getHostBuffer();
    for(int i = 0; i < nbc; i++)
    {
        debugf("original pos[%d]: %f %f %f %f\n", i, cl_particles[i].x, cl_particles[i].y, cl_particles[i].z, cl_particles[i].w);
    }

    //copy all the particles from the GPU to the CPU (num of them) and make it
    //a blocking operation
    cl_particles.copyToHost(num,true);

    //print first few particlesafter 
    //vector<float4>& newpos = cl_particles->getHostBuffer();
    for(int i = 0; i < nbc; i++)
    {
        debugf("new pos[%d]: %f %f %f %f\n", i, cl_particles[i].x, cl_particles[i].y, cl_particles[i].z, cl_particles[i].w);
    }

    exit(0);
}


Grid SetupGrid()
{
    float4 min = float4(-10., -10., -10., 0.);
    float4 max = float4(10., 10., 10., 0.);
    int4 res = int4(50, 50, 50, 0);
    Grid grid(min, max, res);
    return grid;
}
