#ifndef FTLECL_PERMUTE_H_INCLUDED
#define FTLECL_PERMUTE_H_INCLUDED

//SHOULDNT BE COMMON

#include "CLL.h"
#include "Buffer.h"
#include "Kernel.h"

#include "structs.h"
#include "grid.h"

using namespace enjacl;

//namespace rtps
//{
    class Permute 
    {
        public:
            Permute() { cli = NULL;}; //timer = NULL; };
            Permute(std::string path, CL* cli);//, EB::Timer* timer);
            void execute(int num,
                    //input
                    Buffer<float4>& pos_u,
                    Buffer<float4>& pos_s,
                    Buffer<unsigned int>& indices
                    );
                    //params
                    //Buffer<SPHParams>& sphp,
                    /*
                    Buffer<Grid>& gp,
                    //debug params
                    Buffer<float4>& clf_debug,
                    Buffer<int4>& cli_debug
                    */

        private:
            CL* cli;
            Kernel k_permute;
            //EB::Timer* timer;
    };
//}

#endif
