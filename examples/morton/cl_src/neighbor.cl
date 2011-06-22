#ifndef _NEIGHBOR_CL_H_
#define _NEIGHBOR_CL_H_

#include "grid.h"
#include "macros.h"

#define oob(X) ( X < 0 || X >= gp->nb_cells )
//----------------------------------------------------------------------
//Example kernel which fills a neighborlist 

__kernel void neighbor(
                  //__global float4* vars_unsorted,
                  int num,
                  __global float4* pos_s,
                  __global int* nnlist,
                  __constant struct Grid* gp,
                  int ni,
                  float search_radius,
                  int maxnn
                  DEBUG_ARGS
                  //__global float4* fdebug,
                  //__global int4* idebug
                  )
{
    // particle index
    uint i = get_global_id(0);
    if (i >= num) return;
    if (i != ni) return;
    // particle position
    float4 pi = pos_s[i]; 

    int4 cim1 = calcGridCell(pi, gp);
    cim1 = cim1 - 1;
    int4 cip1 = calcGridCell(pi, gp);
    cip1 = cip1 + 1;

    int hashmin = calcMorton(cim1,gp);
    int hashmax = calcMorton(cip1,gp);

    if oob(hashmin)
        hashmin = 0;
    if oob(hashmax)
        hashmax = gp->nb_cells - 1;

    int count = 0;
    //int j = i+1;
    int j = i;
    float4 pj;// = pos_s[j];
    int hashj;// = calcMorton(calcGridCell(pj, gp),gp);
    float4 r;
    float rlen;
    do
    {
        j++;
        if(j >= num) break;
        pj = pos_s[j];
        hashj = calcMorton(calcGridCell(pj,gp),gp);

        r = pj - pi;
        r.w = 0.f;
        rlen = length(r);
        //neighbor stuff
        if (rlen < search_radius)
        {
            //if(count > 50) return; //arbitrary cutoff while we are testing
            if(count < maxnn) //dunno why i'm having problem breaking/returning out of loops
            {
                nnlist[count] = j;
                count++;
            }
        }
        cli[j].x = hashj;
        clif[j].x = rlen;
    } while( hashj <= hashmax );
    nnlist[298] = count;

    //j = i-1;
    j = i;
    do
    {
        j--;
        //if(j < 0) j = 0;
        if(j < 0) break;
        pj = pos_s[j];
        hashj = calcMorton(calcGridCell(pj,gp), gp);

        r = pj - pi;
        r.w = 0.f;
        rlen = length(r);
        //neighbor stuff
        if (rlen < search_radius)
        {
            //if(count > 50) return; //arbitrary cutoff while we are testing
            if(count < maxnn) //dunno why i'm having problem breaking/returning out of loops
            {
                nnlist[count] = j;
                count++;
            }
        }
    } while( hashj >= hashmin);
    nnlist[299] = maxnn;

}
//----------------------------------------------------------------------


#endif
