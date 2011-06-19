#include "grid.h"

typedef unsigned int uint;


inline uint dilate_3(uint t) 
{ 
    //from Raman and Wise: Converting to and From Dilated Integers
    uint r = t;
    r = (r * 0x10001) & 0xFF0000FF; 
    r = (r * 0x00101) & 0x0F00F00F; 
    r = (r * 0x00011) & 0xC30C30C3; 
    r = (r * 0x00005) & 0x49249249; return(r);
}

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


int4 Grid::calcCell(float4 p)
{
    // subtract grid_min (cell position) and multiply by delta
    float4 pp;
    pp.x = (p.x-this->min.x) * this->inv_delta.x;
    pp.y = (p.y-this->min.y) * this->inv_delta.y;
    pp.z = (p.z-this->min.z) * this->inv_delta.z;
    pp.w = (p.w-this->min.w) * this->inv_delta.w;

    int4 ii;
    ii.x = (int) pp.x;
    ii.y = (int) pp.y;
    ii.z = (int) pp.z;
    ii.w = (int) pp.w;
    return ii;
}

float4 Grid::calcPos(int4 cell)
{
    //get the world space coordinate of the cell
    //not really accounting for center vs. bottom left corner...
    float4 pos;
    pos.x = this->min.x + cell.x * this->delta.x;
    pos.y = this->min.x + cell.y * this->delta.y;
    pos.z = this->min.z + cell.z * this->delta.z;
    pos.w = 0.f;
    return pos;
}

int Grid::calcMorton(int4 cell)
{
    //Notes from Steve Henke
    //Morton ordering
    int4 hash;
    hash.x = dilate_3(cell.x);
    hash.y = dilate_3(cell.y);
    hash.z = dilate_3(cell.z);
    return hash.x | hash.y << 1 | hash.z << 2;
}

int Grid::calcHashMax(int hashmin)
{
    //unneccessary, can just add 1 to the cell and hash that
    //we want to add to a dilated integer
    uint xmask = 73;    //masks only the x component
    uint ymask = 146;   //masks only the y compenent
    uint zmask = 292;   //masks only the z component
    uint hashmax = 0;
    uint xtmp = (hashmin | ~xmask) + 1;
    uint ytmp = (hashmin | ~ymask) + 1;
    uint ztmp = (hashmin | ~zmask) + 1;
    hashmax |= xtmp & xmask;
    hashmax |= ytmp & ymask;
    hashmax |= ztmp & zmask;
    return hashmax;

}

    
int Grid::calcCartesian(int4 cell, bool wrapEdges)
{
    //Cartesian strided access
    int gx;
    int gy;
    int gz;

    if (wrapEdges)
    {
        int gsx = (int)floor(res.x);
        int gsy = (int)floor(res.y);
        int gsz = (int)floor(res.z);
        gx = cell.x % gsx;
        gy = cell.y % gsy;
        gz = cell.z % gsz;
        if (gx < 0) gx+=gsx;
        if (gy < 0) gy+=gsy;
        if (gz < 0) gz+=gsz;
    }
    else
    {
        gx = cell.x;
        gy = cell.y;
        gz = cell.z;
    }
    return (gz*res.y + gy) * res.x + gx; 
}





}

