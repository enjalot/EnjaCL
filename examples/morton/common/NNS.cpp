#include "NNS.h"

using namespace std;


vector<float4> SortSeedsCPU(vector<float4> seeds, Grid grid)
{
    sort(seeds.begin(), seeds.end(), grid);
    return seeds;
}

vector<unsigned int> SortHashesCPU(vector<unsigned int> hashes)
{
    sort(hashes.begin(), hashes.end());
    return hashes;
}

vector<unsigned int> SortIndicesCPU(vector<unsigned int> indices, vector<float4> seeds, IndexSorter ind)
{
    sort(indices.begin(), indices.end(), ind);
    return indices;
}


NNS::NNS(vector<float4> seeds_, Grid grd)
{
    printf("GPU with OpenCL\n");
    this->seeds = &seeds_;
    this->grid = grd;

    CL cli;
    string cl_path(MORTON_SOURCE_DIR);

    //Obtain GPU devices available on this system
    vector<EnjaDevice>& devs = cli.getEnjaDevices(CL_DEVICE_TYPE_GPU);
    //choose the first one
    ed = &devs[0];



    //------------------------------------------------------
    //initialize the opencl buffers

    num = seeds->size();
    maxnum = nlpo2(num);
    printf("num %d maxnum %d\n", num, maxnum);
    //use unsorted original seeds
    cl_seeds = Buffer<float4>(ed, seeds);
    cl_seeds_s = Buffer<float4>(ed, seeds);
    //original seeds
    //Buffer<float4> cl_oseeds = Buffer<float4>(cli, seeds);
    //Buffer<float4> cl_vels = Buffer<float4>(cli, vels);
    vector<Grid> vgrid(0);
    vgrid.push_back(grid);
    cl_grid = Buffer<Grid>(ed, &vgrid);

    //Debugging arrays
    clft.resize(maxnum);
    clit.resize(maxnum);
    clf_debug = Buffer<float4>(ed, &clft);
    cli_debug = Buffer<int4>(ed, &clit);

    //Sorting related buffers
    keys.resize(maxnum);
    //to get around limits of bitonic sort only handling powers of 2
    #include "limits.h"
    std::fill(keys.begin(), keys.end(), INT_MAX);
    cl_sort_indices  = Buffer<unsigned int>(ed, &keys);
    cl_sort_hashes   = Buffer<unsigned int>(ed, &keys);

    cl_sort_output_indices  = Buffer<unsigned int>(ed, &keys);
    cl_sort_output_hashes   = Buffer<unsigned int>(ed, &keys);

    //sort oseeds and seeds by hash, permute them
    //setup Hash and Permute classes (they handle the kernels)
    string path = "../cl_src/";
    hashk = Hash(path, ed);
    permutek = Permute(path, ed);
    neighbork = Neighbor(path, ed);


    //------------------------------------------------------
    //Bitonic Sort
    printf("bitonic sort\n");
    bitonick = Bitonic<unsigned int>(path, ed);
    //maybe cta_size should be passed to the call instead of the constructor
    radixk = Radix<unsigned int>(path, ed, maxnum, 128);

}


void NNS::hash()
{
    printf("hash execute\n");
    hashk.execute(num,
            //cl_vars_unsorted,
            cl_seeds,
            cl_sort_hashes,
            cl_sort_indices,
            //cl_sphp,
            cl_grid,
            clf_debug,
            cli_debug);

#if 0
    //check if unsorted hashes from cpu = gpu
    //seems ok
    vector<unsigned int> gputmp(keys.size());
    cl_sort_hashes.copyToHost(gputmp);
    for(int i = 0; i < 50; i++)
    {
        printf("cpu hash: %d | gpu hash: %d\n", hashes[i], gputmp[i]);
    }


#endif

#if 0
    //sorting seems to work fine
    vector<unsigned int> hashind;
    for(int i = 0; i < hashes.size(); i++)
    {
        //hashind.push_back(hashes.size() - i);
        hashind.push_back(i);
    }
    cl_sort_hashes.copyToDevice(hashind);
    cli->queue.finish();

#endif


}

void NNS::radix()
{
    printf("radix sort\n");
    try
    {
        radixk.sort(nlpo2(num), &cl_sort_hashes, &cl_sort_indices);
    }
    catch (cl::Error er)
    {
        printf("ERROR(radix sort): %s(%s)\n", er.what(), oclErrorString(er.err()));
        exit(0);
    }

}
void NNS::bitonic()
{
    try
    {
        printf("bitonic sort\n");
        int dir = 1;// dir: direction
        //NEEDS TO BE POWER OF 2
        int arrayLength = nlpo2(num);
        printf("arrayLength: %d\n", arrayLength);
        printf("num: %d\n", num);
        int batch = 1;
        bitonick.Sort(batch,
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

    //cli->queue.finish();
    printf("copy the results\n");
    cl_sort_hashes.copyFromBuffer(cl_sort_output_hashes, 0, 0, num);
    cl_sort_indices.copyFromBuffer(cl_sort_output_indices, 0, 0, num);
    //cli->queue.finish();
 
}


void NNS::permute()
{
#if 1
    printf("permute!\n");
    permutek.execute(num,
            cl_seeds,
            cl_seeds_s,
            cl_sort_indices);
            //cl_grid,
            //clf_debug,
            //cli_debug);

#endif


}


vector<int> NNS::neighbors(int ni, float search_radius)
{

    int maxnn = 300; 
    vector<int> nnlist(maxnn);
    Buffer<int> cl_nnlist(ed, &nnlist);
    printf("neighbor search\n");
#if 1
    neighbork.execute(num,
            cl_seeds_s,
            cl_nnlist,
            cl_grid,
            ni,
            search_radius,
            maxnn,
            clf_debug,
            cli_debug
            );
#endif

    //cl_nnlist.copyToHost(nnlist);
    cl_nnlist.copyToHost(maxnn, true);
    int tmp = nnlist[298];
    printf("count up gpu: %d\n", tmp);
    tmp = nnlist[299];
    printf("count down gpu: %d\n", tmp);
    return nnlist;

    /*
    nnlist.resize(brute_list.size());//cheat to get the actual # of neighbors
    sort(brute_list.begin(), brute_list.end());
    sort(morton_list.begin(), morton_list.end());
    sort(nnlist.begin(), nnlist.end());
    */

#if 0
    for(int i = 0; i < brute_list.size(); i++)
    {
        printf("brute cpu: %d morton cpu: %d morton gpu: %d\n", brute_list[i], morton_list[i], nnlist[i]);
    }
#endif

#if 0
    cli_debug.copyToHost(clit);
    vector<float4> clft(maxnum);
    clf_debug.copyToHost(clft);
    for(int i = 0; i < 250; i++)
    {
        //printf("hash gpu: %d hash cpu: %d\n", clit[i].x, sorted[i] );
        printf("dist: %f cpu dist: %f\n", clft[i].x, clf[i].x);
    }
    printf("search radius: %f\n", search_radius);
#endif



}



