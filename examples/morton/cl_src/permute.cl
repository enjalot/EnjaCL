// This software contains source code provided by NVIDIA Corporation.
// Specifically code from the CUDA 2.3 SDK "Particles" sample

#ifndef _PERMUTE_
#define _PERMUTE_

#include "grid.h"
#include "macros.h"

//----------------------------------------------------------------------
__kernel void permute(
                            int num,
                            __global float4* pos_u,
                            __global float4* pos_s,
                            __global uint* sort_indices
                            )
{
    uint index = get_global_id(0);
    //int num = sphp->num;
    if (index >= num) return;
    uint sorted_index = sort_indices[index];
    pos_s[index] = (float4)(1.f, 1.f, 1.f, 0.f);
    pos_s[index]     = pos_u[sorted_index];
}
//----------------------------------------------------------------------

#endif
