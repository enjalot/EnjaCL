#include "Permute.h"

#include <string>

//namespace rtps
//{

    //Permute::Permute(std::string path, CL* cli_, EB::Timer* timer_)
    Permute::Permute(std::string path, CL* cli_)
    {
        cli = cli_;
        //timer = timer_;
        printf("create permute kernel\n");
        path = path + "/permute.cl";
        k_permute = Kernel(cli, path, "permute");
        
    }

    void Permute::execute(int num,
                    //input
                    Buffer<float4>& pos_u,
                    Buffer<float4>& pos_s,
                    Buffer<unsigned int>& indices)
                    /*
                    Buffer<Grid>& gp,
                    //debug params
                    Buffer<float4>& clf_debug,
                    Buffer<int4>& cli_debug)
                    */
    {

        
        int iarg = 0;
        k_permute.setArg(iarg++, num);
        k_permute.setArg(iarg++, pos_u.getDevicePtr());
        k_permute.setArg(iarg++, pos_s.getDevicePtr());
        k_permute.setArg(iarg++, indices.getDevicePtr());

        int workSize = 128;
        
        //printf("about to data structures\n");
        try
        {
            float gputime = k_permute.execute(num, workSize);
            //if(gputime > 0)
            //    timer->set(gputime);

        }
        catch (cl::Error er)
        {
            printf("ERROR(data structures): %s(%s)\n", er.what(), oclErrorString(er.err()));
        }

        
#if 0
        //printPermuteDiagnostics();

        printf("**************** Permute Diagnostics ****************\n");
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
