#include <stdio.h>
#include <iostream>
#include <vector>


//enjacl
#include "CLL.h"
#include "Buffer.h"
#include "Kernel.h"
#include "common/Hash.h"
#include "common/Permute.h"
#include "common/BitonicSort.h"
#include "util.h"
#include "grid.h"
#include "structs.h"




using namespace std;
using namespace enjacl;

void putbits(int word)
{
    //This function comes from Steve Henke's code
    const unsigned int nbit = CHAR_BIT*sizeof(int);
    for (unsigned int bit=0; bit<nbit; ++bit)
    {
        if (bit % CHAR_BIT == 0u) std::cout.put(' ');
            std::cout << (word >> (nbit-bit-1u) & int(1));
    }
    printf("\n");
}

void print_stuff(float4 p, Grid* grid)
{
    p.print("p");
    int4 c = grid->calcCell(p);
    c.print("c");
    unsigned int h = grid->calcMorton(c);
    putbits(h);
}

vector<int> nn_bruteforce(int i, vector<float4> particles, float search_radius)
{
    float4 pi = particles[i];
    vector<int> nearestn;
    for(int j = 0; j < particles.size(); j++)
    {
        if (i == j) continue;
        float4 pj = particles[j];
        if (magnitude(pj-pi) < search_radius)
        {
            nearestn.push_back(j);
        }

    }
    return nearestn;
        
}

#define oob(X) ( X < 0 || X >= grid.nb_cells )
vector<int> nn_morton(int i, vector<float4> particles, float search_radius, Grid grid)
{
    float4 pi = particles[i];
    vector<int> nearestn;

    int4 cim1 = grid.calcCell(pi);
    cim1.print("ci");
    cim1 = cim1 - 1;
    cim1.print("cim1");
    int4 cip1 = grid.calcCell(pi);
    cip1 = cip1 + 1;
    cip1.print("cip1");

    int hashmin = grid.calcMorton(cim1);
    int hashmax = grid.calcMorton(cip1);
    printf("hashmin %d\n", hashmin);
    printf("hashmax %d\n", hashmax);

    if oob(hashmin)
        hashmin = 0;
    if oob(hashmax)
        hashmax = grid.nb_cells - 1;

    printf("i = %d\n", i);
    //todo check for j in bounds
    int j = i+1;
    float4 pj = particles[j];
    int hashj = grid.calcMorton(grid.calcCell(pj));
    printf("hashj %d\n", hashj);
    printf("hashmin %d\n", hashmin);
    printf("hashmax %d\n", hashmax);
    while( hashj <= hashmax )
    {
        printf("to max: j = %d\n", j);
        //neighbor stuff
        if (magnitude(pj-pi) < search_radius)
        {
            nearestn.push_back(j);
        }
        j++;
        if(j >= particles.size())
            break;
        pj = particles[j];
        printf("hashj %d\n", hashj);
        hashj = grid.calcMorton(grid.calcCell(pj));
    }

    j = i-1;
    pj = particles[j];
    hashj = grid.calcMorton(grid.calcCell(pj));
    while( hashj >= hashmin)
    {
        printf("to min: j = %d\n", j);
        //neighbor stuff
        if (magnitude(pj-pi) < search_radius)
        {
            nearestn.push_back(j);
        }
        j--;
        if(j < 0)
            break;

        pj = particles[j];
        int4 cj = grid.calcCell(pj);
        cj.print("cj");
        hashj = grid.calcMorton(grid.calcCell(pj));
        printf("hashj %d\n", hashj);
    }
    return nearestn;
 
}


int main()
{
    //make grid
    float4 min = float4(0, 0, 0, 0);
    float4 max = float4(1, 1, 1, 0);
    int4 res = int4(4, 4, 4, 0);
    Grid grid(min, max, res);
    grid.print();
 
    //make 4x as many particles with a grid who'se cells are 1/4 the size of the main grid
    Grid ngrid(min, max, grid.delta / 8.);
    float4 offset = ngrid.delta / 2.;
    vector<float4> seeds = ngrid.plantSeeds(offset);
    vector<unsigned int> hashes;
    vector<float4> oseeds = seeds;



    printf("CPU morton ordered neighbor search\n");
    int4 c;
    for(int i = 0; i < seeds.size(); i++)
    {
        //print_stuff(seeds[i], &grid);
        c = grid.calcCell(seeds[i]);
        hashes.push_back(grid.calcMorton(c));
    }

    vector<unsigned int> sorted = hashes;
    sort(sorted.begin(), sorted.end());
    sort(seeds.begin(), seeds.end(), grid);

    printf("hashes.size() = %zd\n", hashes.size());
    printf("sorted.size() = %zd\n", sorted.size());
#if 0
    for(int i = 0; i < sorted.size(); i++)
    {
        printf("original hash: %d; sorted hash: %d\n", hashes[i], sorted[i]);
    }
#endif

    c = int4(3,3,3,0);
    printf("hash of cell -1,-1,-1: %d\n", grid.calcMorton(c));
       float search_radius = grid.delta.x / 2.;
    vector<int> brute_list = nn_bruteforce(9, seeds, search_radius);
    vector<int> morton_list = nn_morton( 9, seeds, search_radius, grid);

    printf("brute force list %zd\n", brute_list.size());
    printf("morton list %zd\n", morton_list.size());

#if 0
    for(int i = 0; i < brute_list.size(); i++)
    {
        printf("brute nn %d\n", brute_list[i]);
    }
    for(int i = 0; i < morton_list.size(); i++)
    {
        printf("morton nn %d\n", morton_list[i]);
    }
#endif

    printf("GPU with OpenCL\n");

    CL *cli = new CL();
    //cli->addIncludeDir("../cl_src/");
    //obviously this path should be relative to the executable somehow
    cli->addIncludeDir("/Users/enjalot/code/enjacl/examples/morton/cl_src");
    //setup buffers

    //instantiate hash kernel
    Kernel k_hash;
    string path = "../cl_src/hash.cl";
    k_hash = Kernel(cli, path, "hash");
    
    //instantiate permute kernel
    Kernel k_permute;
    path = "../cl_src/permute.cl";
    k_permute = Kernel(cli, path, "permute");

    //instantiate morton kernel
    Kernel k_morton;
    path = "../cl_src/morton.cl";
    k_morton = Kernel(cli, path, "morton");

    //------------------------------------------------------
    //initialize the opencl buffers

    int num = seeds.size();
    int maxnum = nlpo2(seeds.size());
    printf("num %d maxnum %d\n", num, maxnum);
    //use unsorted original seeds
    Buffer<float4> cl_seeds = Buffer<float4>(cli, oseeds);
    Buffer<float4> cl_seeds_s = Buffer<float4>(cli, oseeds);
    //original seeds
    //Buffer<float4> cl_oseeds = Buffer<float4>(cli, seeds);
    //Buffer<float4> cl_vels = Buffer<float4>(cli, vels);
    vector<Grid> vgrid(0);
    vgrid.push_back(grid);
    Buffer<Grid> cl_grid = Buffer<Grid>(cli, vgrid);

    //Debugging arrays
    Buffer<float4> clf_debug = Buffer<float4>(cli, seeds);
    vector<int4> clit(maxnum);
    Buffer<int4> cli_debug = Buffer<int4>(cli, clit);

    //Sorting related buffers
    std::vector<unsigned int> keys(maxnum);
    //to get around limits of bitonic sort only handling powers of 2
    #include "limits.h"
    std::fill(keys.begin(), keys.end(), INT_MAX);
    Buffer<unsigned int> cl_sort_indices  = Buffer<unsigned int>(cli, keys);
    Buffer<unsigned int> cl_sort_hashes   = Buffer<unsigned int>(cli, keys);

    Buffer<unsigned int> cl_sort_output_indices  = Buffer<unsigned int>(cli, keys);
    Buffer<unsigned int> cl_sort_output_hashes   = Buffer<unsigned int>(cli, keys);

    //sort oseeds and seeds by hash, permute them
    //setup Hash and Permute classes (they handle the kernels)
    path = "../cl_src/";
    Hash hash = Hash(path, cli);
    Permute permute = Permute(path, cli);

    printf("hash execute\n");
    hash.execute(num,
            //cl_vars_unsorted,
            cl_seeds,
            cl_sort_hashes,
            cl_sort_indices,
            //cl_sphp,
            cl_grid,
            clf_debug,
            cli_debug);

    //------------------------------------------------------
    //Bitonic Sort
    printf("bitonic sort\n");
    Bitonic<unsigned int> bitonic = Bitonic<unsigned int>(path, cli);
#if 1
    try
    {
        int dir = 1;// dir: direction
        //NEEDS TO BE POWER OF 2
        int arrayLength = nlpo2(num);
        //printf("arrayLength: %d\n", arrayLength);
        int batch = 1;
        bitonic.Sort(batch,
                    arrayLength,
                    dir,
                    &cl_sort_output_hashes,
                    &cl_sort_output_indices,
                    &cl_sort_hashes,
                    &cl_sort_indices );
    }
    catch (cl::Error er)
    {
        printf("ERROR(bitonic sort): %s(%s)\n", er.what(), oclErrorString(er.err()));
        exit(0);
    }
#endif
    cli->queue.finish();
    printf("copy the results\n");
    cl_sort_hashes.copyFromBuffer(cl_sort_output_hashes, 0, 0, num);
    cl_sort_indices.copyFromBuffer(cl_sort_output_indices, 0, 0, num);
    cli->queue.finish();
    //------------------------------------------------------

#if 1
    printf("permute!\n");
    //sort oseeds into seeds then copy seeds into oseeds!
    permute.execute(num,
            cl_seeds,
            cl_seeds_s,
            cl_sort_indices);
            //cl_grid,
            //clf_debug,
            //cli_debug);

    //cl_seeds.copyFromBuffer(cl_seeds_s, 0, 0, num);
#endif

    





    return 0;
}
