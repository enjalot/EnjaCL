//std
#include <vector>
#include <algorithm>

//enjacl
#include "CLL.h"
#include "Buffer.h"
#include "Kernel.h"
#include "common/Hash.h"
#include "common/Permute.h"
#include "common/BitonicSort.h"
#include "common/Radix.h"
#include "common/Neighbor.h"
#include "util.h"
#include "grid.h"
#include "structs.h"

using namespace std;
using namespace enjacl;

class IndexSorter 
{
    public:
    IndexSorter(vector<float4> pts, Grid grd){ 
        points = pts; grid = grd;
        /*
        for(int i = 0; i < points.size(); i++)
        {
            inds.push_back(i);
        }
        */
    };
    vector<float4> points;
    //vector<unsigned int> inds;
    Grid grid;

    bool operator()(unsigned int i, unsigned int j) { return (this->grid.calcMorton(this->grid.calcCell(points[i])) < this->grid.calcMorton(this->grid.calcCell(points[j]))); }

};


//CPU stuff
vector<float4> SortSeedsCPU(vector<float4> seeds, Grid grid);
vector<unsigned int> SortHashesCPU(vector<unsigned int> hashes);
vector<unsigned int> SortIndicesCPU(vector<unsigned int> indices, vector<float4> seeds, IndexSorter ind);


template <class T>
bool verify_vecs(vector<T> a, vector<T>b)
{
    if(a.size() != b.size())
    {
        printf("size mismatch! a: %zd, b: %zd\n", a.size(), b.size());
        return false;
    }
    int mismatch_count = 0;
    for(int i = 0; i < a.size(); i++)
    {
        if(a[i] != b[i])
        {
            //printf("mismatch at %d\n");
            mismatch_count++;
        }
    }
    if(mismatch_count > 0)
    {
        printf("%d mismatches\n", mismatch_count);
        return false;
    }
    return true;
}


//GPU stuff

class NNS
{
    public:
        NNS(vector<float4> seeds, Grid grid);
        void hash();
        void bitonic();
        void radix();
        void permute();
        vector<int> neighbors(int ni, float search_radius);


    //private:
    public:
        //CL *cli;
        EnjaDevice* ed;
        int num;
        int maxnum;
        vector<float4> *seeds;
        Grid grid;
        Buffer<float4> cl_seeds;
        Buffer<float4> cl_seeds_s;
        Buffer<Grid> cl_grid;

        //Debugging arrays
        vector<float4> clft;
        Buffer<float4> clf_debug;
        vector<int4> clit;
        Buffer<int4> cli_debug;

        //Sorting related buffers
        std::vector<unsigned int> keys;
        Buffer<unsigned int> cl_sort_indices;
        Buffer<unsigned int> cl_sort_hashes;

        Buffer<unsigned int> cl_sort_output_indices;
        Buffer<unsigned int> cl_sort_output_hashes;

        Hash hashk;
        Bitonic<unsigned int> bitonick;
        Radix<unsigned int> radixk;
        Permute permutek;
        Neighbor neighbork;

};


