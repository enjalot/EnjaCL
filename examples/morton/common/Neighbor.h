#ifndef FTLECL_NEIGHBOR_H_INCLUDED
#define FTLECL_NEIGHBOR_H_INCLUDED

//SHOULDNT BE COMMON

#include "CLL.h"
#include "Buffer.h"
#include "Kernel.h"

#include "structs.h"
#include "grid.h"

using namespace enjacl;

//namespace rtps
//{
    class Neighbor 
    {
        public:
            Neighbor() { cli = NULL;}; //timer = NULL; };
            Neighbor(std::string path, CL* cli);//, EB::Timer* timer);
            void execute(int num,
                    //input
                    Buffer<float4>& pos_s,
                    Buffer<int>& nnlist,
                    Buffer<Grid>& gp,
                    int ni,
                    float search_radius,
                    int maxnn,
                    //debug params
                    Buffer<float4>& clf_debug,
                    Buffer<int4>& cli_debug
                    );

        private:
            CL* cli;
            Kernel k_neighbor;
            //EB::Timer* timer;
    };
//}

#endif
