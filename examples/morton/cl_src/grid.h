#ifndef CL_GRID_H
#define CL_GRID_H

struct Grid
{
    float4          size;
    float4          min;
    float4          max;
    float4          bnd_min;
    float4          bnd_max;
    // number of cells in each dimension/side of grid
    float4          res;
    float4          delta;
    float4          inv_delta;
    //float4          grid_inv_delta;
    int nb_cells;

}; 

inline uint dilate_3(uint t) 
{ 
    //from Raman and Wise: Converting to and From Dilated Integers
    uint r = t;
    r = (r * 0x10001) & 0xFF0000FF; 
    r = (r * 0x00101) & 0x0F00F00F; 
    r = (r * 0x00011) & 0xC30C30C3; 
    r = (r * 0x00005) & 0x49249249; return(r);
}

#if 1
int4 calcGridCell(float4 p, __constant struct Grid* grid)
{
    // subtract grid_min (cell position) and multiply by inverse delta
    float4 pp;
    pp.x = (p.x-grid->min.x) * grid->inv_delta.x;
    pp.y = (p.y-grid->min.y) * grid->inv_delta.y;
    pp.z = (p.z-grid->min.z) * grid->inv_delta.z;
    pp.w = (p.w-grid->min.w) * grid->inv_delta.w;

    int4 ii;
    ii.x = (int) pp.x;
    ii.y = (int) pp.y;
    ii.z = (int) pp.z;
    //ii.w = (int) pp.w;
    ii.w = 0;
    return ii;
}
#endif

#if 1
float4 calcGridPos(int4 cell, __constant struct Grid* grid)
{
    //get the world space coordinate of the cell
    //not really accounting for center vs. bottom left corner...
    float4 pos;
    pos.x = grid->min.x + cell.x * grid->delta.x;
    pos.y = grid->min.x + cell.y * grid->delta.y;
    pos.z = grid->min.z + cell.z * grid->delta.z;
    pos.w = 0.f;
    return pos;
}
#endif
    
#if 1
int calcGridHash(int4 cell, float4 grid_res, bool wrapEdges)
{
    int gx;
    int gy;
    int gz;

    if (wrapEdges)
    {
        int gsx = (int)floor(grid_res.x);
        int gsy = (int)floor(grid_res.y);
        int gsz = (int)floor(grid_res.z);
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
    return (gz*grid_res.y + gy) * grid_res.x + gx; 
}
#endif

int calcMorton(int4 cell, __constant struct Grid* grid)
{
    //Implemented based on notes from Steve Henke
    //Morton ordering
    int4 hash;
    hash.x = dilate_3(cell.x);
    hash.y = dilate_3(cell.y);
    hash.z = dilate_3(cell.z);
    return hash.x | hash.y << 1 | hash.z << 2;

}



#endif
