#ifndef ENJACL_GRID_H
#define ENJACL_GRID_H

#include <vector>
#include "structs.h"

namespace enjacl
{

class Grid
{
    public:
        //width, height, depth in units
        float4          size;
        //minimum coordinate
        float4          min;
        //maximum coordinate
        float4          max;
        //bounds for using a subset of the grid (like in collision detection)
        float4          bnd_min;
        float4          bnd_max;
        // number of cells in each dimension/side of grid
        float4          res;
        // width in units of a cell
        float4          delta;
        float4          inv_delta;

        int nb_cells;

        Grid();
        Grid(float4 _min, float4 _max, float4 _delta);
        Grid(float4 _min, float4 _max, int4 _res);

        void print();

        std::vector<float4> plantSeeds( float4 offset );

        int4 calcCell(float4 p);
        float4 calcPos(int4 cell);
        int calcCartesian(int4 cell, bool wrapEdges);
        int calcMorton(int4 cell);
        int calcHashMax(int hashmin);
        
        bool operator()(float4 pi,float4 pj) { return (this->calcMorton(this->calcCell(pi)) < this->calcMorton(this->calcCell(pj))); }
        

}; 
}
#endif
