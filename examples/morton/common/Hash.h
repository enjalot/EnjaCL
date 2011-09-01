#ifndef FTLECL_HASH_H_INCLUDED
#define FTLECL_HASH_H_INCLUDED

#include "CLL.h"
#include "Buffer.h"
#include "Kernel.h"

#include "structs.h"
#include "grid.h"

//eventually move these common classes into EnjaCL?
using namespace enjacl;

//namespace rtps
//{
    class Hash
    {
        public:
            Hash() { ed = NULL; };//timer = NULL; };
            Hash(std::string path, EnjaDevice* ed);
            //Hash(std::string path, CL* cli, EB::Timer* timer);
            void execute(int num,
                        //input
                        Buffer<float4>& pos_u, 
                        //output
                        Buffer<unsigned int>& hashes,
                        Buffer<unsigned int>& indices,
                        //params
                        Buffer<Grid>& grid,
                        //debug
                        Buffer<float4>& clf_debug,
                        Buffer<int4>& cli_debug);
            
           

        private:
            EnjaDevice* ed;
            Kernel k_hash;
            //EB::Timer* timer;
    };
//}

#endif
