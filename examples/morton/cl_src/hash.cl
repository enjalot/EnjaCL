#ifndef _HASH_CL_H_
#define _HASH_CL_H_

#include "grid.h"
#include "macros.h"

//----------------------------------------------------------------------
// Calculate a grid hash value for each particle

__kernel void hash(
                  //__global float4* vars_unsorted,
                  int num,
                  __global float4* pos_u,
                  __global uint* sort_hashes,
                  __global uint* sort_indexes,
                  //__constant struct SPHParams* sphp,
                  __constant struct Grid* gp
                  DEBUG_ARGS
                  //__global float4* fdebug,
                  //__global int4* idebug
                  )
{
    // particle index
    uint index = get_global_id(0);
    //comment this out to hash everything if using max_num
    if (index >= num) return;

    // particle position
    float4 p = pos_u[index]; // macro

    // get address in grid
    int4 gridPos = calcGridCell(p, gp);
    bool wrap_edges = false;
    //uint hash = (uint) calcGridHash(gridPos, gp->grid_res, wrap_edges);//, fdebug, idebug);
    //int hash = calcGridHash(gridPos, gp->res, wrap_edges);//, fdebug, idebug);
    int hash = calcMorton(gridPos, gp);

    cli[index].xyz = gridPos.xyz;
    cli[index].w = hash;
    //cli[index].w = (gridPos.z*gp->grid_res.y + gridPos.y) * gp->grid_res.x + gridPos.x; 

    hash = hash > gp->nb_cells ? gp->nb_cells : hash;
    hash = hash < 0 ? gp->nb_cells : hash;
    // store grid hash and particle index
    sort_hashes[index] = (uint)hash;
    sort_indexes[index] = index;

    //clf[index] = (float4)((p.x - gp->grid_min.x) * gp->grid_delta.x, p.x, 0,0);
    //clf[index] = p;

}
//----------------------------------------------------------------------


#endif
