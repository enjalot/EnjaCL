#include "grid.h"

namespace enjacl
{

Grid::Grid()
{
}

Grid::Grid(float4 _min, float4 _max, float4 _delta)
{
    //Make a grid from min, max and the size of each cell
    //this may change the max if size (distance between min and max) is not a
    //multiple of delta
    // (it should be in our simplest rectilinear examples where our particles
    // are spaced evenly, and the same as the vector field)
    min = _min;
    max = _max;
    delta = _delta;
    size = max - min;

    res = float4(ceil(size.x/delta.x),
                 ceil(size.y/delta.y),
                 ceil(size.z/delta.z),
                 0.0f);

    size = res * delta;
    delta = size / res;
    inv_delta = float4(1., 1., 1., 1.) / delta;
    max = min + size;

    nb_cells = res.x * res.y * res.z;
    //bnd_min
    //bnd_max

}

Grid::Grid(float4 _min, float4 _max, int4 _res)
{
    //Make a grid from min, max and the number of cells in each direction
    min = _min;
    max = _max;
    res = float4(_res.x, _res.y, _res.z, 0.0f);
    size = max - min;
    delta = size / res;
    inv_delta = float4(1., 1., 1., 1.) / delta;
    nb_cells = res.x * res.y * res.z;
    //bnd_min
    //bnd_max

}

std::vector<float4> Grid::plantSeeds( float4 offset )
{
    //initialize a seed in every cell of the grid
    //offset: amount to offset from the min corner (bottom left) e.g. place seeds in center of cell
    //with offset = .5 * delta;
                 
    //std::vector<float4> rvec(num);
    float4 tmin = min + offset;
    std::vector<float4> rvec;
    int i=0;
    for (float z = tmin.z; z <= max.z ; z += delta.z)
    {            
        for (float y = tmin.y; y <= max.y; y += delta.y)
        {        
            for (float x = tmin.x; x <= max.x; x += delta.x)
            {
                //if (i >= num) break;
                //rvec[i] = float4(x,y,z,1.0f);
                rvec.push_back(float4(x,y,z,1.0f));
                i++;
            }     
        }
    }
    //rvec.resize(i);
    return rvec;   
 

}


void Grid::print()
{
    printf("-----Grid----\n");
    min.print("grid min");
    max.print("grid max");
    bnd_min.print("bnd_min");
    bnd_max.print("bnd_max");
    res.print("grid res");
    size.print("grid size");
    delta.print("grid delta");
    inv_delta.print("grid_inv_delta"); 
    printf("nb grid cells: %d\n", nb_cells);
}

}

