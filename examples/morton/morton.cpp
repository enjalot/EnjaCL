#include <stdio.h>
#include <iostream>
#include <vector>


//enjacl
#include "grid.h"
#include "structs.h"

using namespace std;
using namespace enjacl;

void putbits(int word)
{
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
    Grid ngrid(min, max, grid.delta / 4.);
    float4 offset = ngrid.delta / 2.;
    vector<float4> seeds = ngrid.plantSeeds(offset);
    vector<unsigned int> hashes;

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

    printf("hashes.size() = %d\n", hashes.size());
    printf("sorted.size() = %d\n", sorted.size());
    for(int i = 0; i < sorted.size(); i++)
    {
        printf("original hash: %d; sorted hash: %d\n", hashes[i], sorted[i]);
    }

    c = int4(3,3,3,0);
    printf("hash of cell -1,-1,-1: %d\n", grid.calcMorton(c));
       float search_radius = grid.delta.x / 2.;
    vector<int> brute_list = nn_bruteforce(9, seeds, search_radius);
    vector<int> morton_list = nn_morton( 9, seeds, search_radius, grid);

    printf("brute force list %d\n", brute_list.size());
    printf("morton list %d\n", morton_list.size());

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



    /*
    unsigned int hmax = grid.calcHashMax(h);
    putbits(hmax);

    somec.x++;
    somec.y++;
    somec.z++;
    h = grid.calcMorton(somec);
    putbits(h);
    */


    /*
    unsigned int xmask = 73; 
    unsigned int ymask = 146;
    unsigned int zmask = 292;
    putbits(xmask);
    putbits(ymask);
    putbits(zmask);
    */


    


    

    return 0;
}
