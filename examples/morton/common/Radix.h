#ifndef ENJACL_RADIX_SORT_H
#define ENJACL_RADIX_SORT_H

#include "CLL.h"
#include "Kernel.h"
#include "Buffer.h"

#ifndef uint
#define uint unsigned int
#endif


namespace enjacl
{

template <class T>
class Radix 
{
public:
    static const uint LOCAL_SIZE_LIMIT = 512U;
    Radix(){ cli=NULL; };
    //create an OpenCL buffer from existing data
    Radix( std::string source_dir, CL *cli, int cta_size );

    int Sort(int batch, int arrayLength, int dir,
                Buffer<T> *dstkey, Buffer<T> *dstval, 
                Buffer<T> *srckey, Buffer<T> *srcval);
    void loadKernels(std::string source_dir);



private:
    Kernel k_scanNaive;
    Kernel k_radixSortBlockKeysValues; 
    Kernel k_radixSortBlocksKeysValues;
    Kernel k_reorderDataKeysValues;
    Kernel k_findRadixOffsets;

    CL *cli;

    int WARP_SIZE;
    int SCAN_WG_SIZE;
    int MIN_LARGE_ARRAY_SIZE;
    int bit_step;
    int cta_size;
    size_t uintsz(T);

};


#include "Radix.cpp"
}
#endif
