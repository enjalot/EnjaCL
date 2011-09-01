#include "Neighbor.h"

#include <string>

//namespace rtps
//{

    //Neighbor::Neighbor(std::string path, CL* cli_, EB::Timer* timer_)
    Neighbor::Neighbor(std::string path, EnjaDevice* ed_)
    {
        ed = ed_;
        //timer = timer_;
        printf("create neighbor kernel\n");
        path = path + "/neighbor.cl";
        k_neighbor = Kernel(ed, path, "neighbor");
        
    }

    void Neighbor::execute(int num,
                    Buffer<float4>& pos_s,
                    Buffer<int>& nnlist,
                    Buffer<Grid>& gp,
                    int ni,
                    float search_radius,
                    int maxnn,
                    Buffer<float4>& clf_debug,
                    Buffer<int4>& cli_debug
                    ) 
    {

        
        int iarg = 0;
        k_neighbor.setArg(iarg++, num);
        k_neighbor.setArg(iarg++, pos_s.getBuffer());
        k_neighbor.setArg(iarg++, nnlist.getBuffer());
        k_neighbor.setArg(iarg++, gp.getBuffer());
        k_neighbor.setArg(iarg++, ni);
        k_neighbor.setArg(iarg++, search_radius);
        k_neighbor.setArg(iarg++, maxnn);
    
        //debug
        k_neighbor.setArg(iarg++, clf_debug.getBuffer());
        k_neighbor.setArg(iarg++, cli_debug.getBuffer());



        int workSize = 128;
        try
        {
            k_neighbor.execute(num, workSize);
        }
        catch (cl::Error er)
        {
            printf("ERROR(data structures): %s(%s)\n", er.what(), oclErrorString(er.err()));
        }

        
#if 0
        //printNeighborDiagnostics();

        printf("**************** Neighbor Diagnostics ****************\n");
        int nbc = nb_cells + 1;
        printf("nb_cells: %d\n", nbc);
        printf("num particles: %d\n", num);

        std::vector<unsigned int> is(nbc);
        std::vector<unsigned int> ie(nbc);
        
        ci_end.copyToHost(ie);
        ci_start.copyToHost(is);


        for(int i = 0; i < nbc; i++)
        {
            if (is[i] != -1)// && ie[i] != 0)
            {
                //nb = ie[i] - is[i];
                //nb_particles += nb;
                printf("cell: %d indices start: %d indices stop: %d\n", i, is[i], ie[i]);
            }
        }

#endif

#if 0
        //print out elements from the sorted arrays
#define DENS 0
#define POS 1
#define VEL 2

            nbc = num+5;
            std::vector<float4> poss(nbc);
            std::vector<float4> dens(nbc);

            //svars.copyToHost(dens, DENS*sphp.max_num);
            svars.copyToHost(poss, POS*sphp.max_num);

            for (int i=0; i < nbc; i++)
            //for (int i=0; i < 10; i++) 
            {
                poss[i] = poss[i] / sphp.simulation_scale;
                //printf("-----\n");
                //printf("clf_debug: %f, %f, %f, %f\n", clf[i].x, clf[i].y, clf[i].z, clf[i].w);
                printf("pos sorted: %f, %f, %f, %f\n", poss[i].x, poss[i].y, poss[i].z, poss[i].w);
                //printf("dens sorted: %f, %f, %f, %f\n", dens[i].x, dens[i].y, dens[i].z, dens[i].w);
            }

#endif


        //return nc;
    }

//}
