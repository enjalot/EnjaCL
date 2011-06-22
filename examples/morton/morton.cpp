#include <stdio.h>
#include <iostream>
#include <vector>


//enjacl
#include "common/NNS.h"

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

vector<float4> clf;

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
    //printf("hashmin %d\n", hashmin);
    //printf("hashmax %d\n", hashmax);

    if oob(hashmin)
        hashmin = 0;
    if oob(hashmax)
        hashmax = grid.nb_cells - 1;

    printf("i = %d\n", i);
    //todo check for j in bounds
    int j = i;
    float4 pj;// = particles[j];
    int hashj;// = grid.calcMorton(grid.calcCell(pj));
    //printf("hashj %d\n", hashj);
    int count = 0;
    printf("hashmin %d\n", hashmin);
    printf("hashmax %d\n", hashmax);
    do
    {
        j++;
        if(j >= particles.size())
            break;
        pj = particles[j];
        //printf("hashj %d\n", hashj);
        hashj = grid.calcMorton(grid.calcCell(pj));

        //printf("to max: j = %d\n", j);
        //neighbor stuff
        clf.push_back( float4(magnitude(pj-pi), 0, 0, 0));
        if (magnitude(pj-pi) < search_radius)
        {
            nearestn.push_back(j);
            count++;
        }
    } while( hashj <= hashmax );
    printf("count: %d\n", count);

    j = i;
    do
    {
        j--;
        if(j < 0)
            break;
        pj = particles[j];
        //int4 cj = grid.calcCell(pj);
        //cj.print("cj");
        hashj = grid.calcMorton(grid.calcCell(pj));
        //printf("hashj %d\n", hashj);

        //printf("to min: j = %d\n", j);
        //neighbor stuff
        if (magnitude(pj-pi) < search_radius)
        {
            nearestn.push_back(j);
            count++;
        }
    } while( hashj >= hashmin);
    printf("count: %d\n", count);
    return nearestn;
 
}


int main()
{
    //make grid
    float4 min = float4(0, 0, 0, 0);
    float4 max = float4(1, 1, 1, 0);
    //int4 res = int4(4, 4, 4, 0);
    int4 res = int4(2, 1, 1, 0);
    Grid grid(min, max, res);
    grid.print();
 
    //make 4x as many particles with a grid who'se cells are 1/4 the size of the main grid
    Grid ngrid(min, max, grid.delta / 8.);
    //Grid ngrid(min, max, grid.delta / 4.);
    float4 offset = ngrid.delta / 2.;
    //if offset == 0 we can lose neighbors in some cases (should check into
    //what happens if particles are on the hash border)
    //float4 offset = float4(0.001, 0, 0, 0);
    vector<float4> seeds = ngrid.plantSeeds(offset);
    vector<unsigned int> hashes;
    vector<float4> sorted_seeds = seeds;


    //initialize the nearest neighbor class for OpenCL search
    NNS nns(seeds, grid);


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
    sort(sorted_seeds.begin(), sorted_seeds.end(), grid);

    printf("hashes.size() = %zd\n", hashes.size());
    printf("sorted.size() = %zd\n", sorted.size());
#if 0
    for(int i = 0; i < sorted.size(); i++)
    {
        printf("original hash: %d; sorted hash: %d\n", hashes[i], sorted[i]);
    }
#endif

    //c = int4(3,3,3,0);
    //printf("hash of cell -1,-1,-1: %d\n", grid.calcMorton(c));
    
    float search_radius = grid.delta.x / 2.;
    int ni = 0;
    vector<int> brute_list = nn_bruteforce(ni, sorted_seeds, search_radius);
    vector<int> morton_list = nn_morton(ni, sorted_seeds, search_radius, grid);

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

    nns.hash();
    nns.bitonic();
    nns.permute();
    vector<int> nnlist = nns.neighbors(ni, search_radius);


   //------------------------------------------------------
#if 0
    //check if sorted hashes from cpu = gpu
    //seems ok
    vector<unsigned int> gputmp(keys.size());
    cl_sort_hashes.copyToHost(gputmp);
    for(int i = 0; i < sorted.size(); i++)
    {
        //printf("cpu hash: %d | gpu hash: %d\n", sorted[i], gputmp[i]);
        if(sorted[i] != gputmp[i])
        {
            printf("%d doesn't agree!\n", i);
        }
    }
#endif

#if 0
    //check if sorted hashes from cpu = gpu
    //seems ok
    vector<unsigned int> gpuind(keys.size());
    cl_sort_indices.copyToHost(gpuind);
    vector<float4> tmpseeds(seeds.size());
    vector<unsigned int> tmphash(keys.size());
    for(int i = 0; i < oseeds.size(); i++)
    {
        tmpseeds[i] = oseeds[gpuind[i]];
        //printf("gpu index: %d\n",  gpuind[i]);
    }
    for(int i = 0; i < hashes.size(); i++)
    {
        tmphash[i] = hashes[gpuind[i]];
    }
    for(int i = 0; i < sorted.size(); i++)
    {
        //printf("cpu hash: %d | gpu hash: %d\n", sorted[i], gputmp[i]);
        if(sorted[i] != tmphash[i])
    j    {
            printf("%d doesn't agree!\n", i);
        }
    }

    for(int i = 0; i < 50; i++)
    {
        oseeds[i].print("cpu");
        tmpseeds[i].print("gpu");
        printf("---------------\n");
    }

    printf("trying to sort index array on cpu\n");
    //TempHI tmphi(oseeds, grid);
    IndexSorter ind(oseeds, grid);
    vector<unsigned int> hashind;
    for(int i = 0; i < hashes.size(); i++)
    {
        //hashind.push_back(hashes.size() - i);
        hashind.push_back(i);
    }
    //sort(hashind.begin(), hashind.end(), tmphi);
    for(int i = 0; i < 50; i++)
    {
        printf("cpu ind: %d\n", hashind[i]);
    }



#endif

        //permute
#if 0
    //double check that cl_seeds_s is sorted version of seeds
    cl_seeds_s.copyToHost(oseeds);
    for(int i = 0; i < 250; i++)
    {
        seeds[i].print("cpu");
        oseeds[i].print("gpu");
        printf("---------------\n");
    }
#endif



    return 0;
}
