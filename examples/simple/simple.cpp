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


    Grid grid = SetupGrid();
    grid.print();
    //initialize particles in a grid, one at the center of each grid cell
    float4 offset = .5 * grid.delta;
    vector<float4> particles = grid.plantSeeds( offset );
    int num = particles.size();
    printf("Num Particles: %zd\n", num)
    //Setup the OpenCL Buffer
    Buffer<float4> cl_particles = Buffer<float4>(cli, particles);

    float dt = .1f;
    float4 v = float4(.5, .5, .5, 0.);

    int iarg = 0;
    k_simple.setArg(iarg++, cl_particles.getDevicePtr());
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
    for(int i = 0; i < nbc; i++)
    {
        printf("original pos[%d]: %f %f %f %f\n", j, particles[j].x, particles[j].y, particles[j].z, particles[j].w);
    }
    cl_particles.copyToHost(particles);
    //print first few particles before
    for(int i = 0; i < nbc; i++)
    {
        printf("new pos[%d]: %f %f %f %f\n", j, particles[j].x, particles[j].y, particles[j].z, particles[j].w);
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
