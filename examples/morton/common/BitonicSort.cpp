
/**
 * C++ port of NVIDIA's Bitonic Sort implementation
 */


template <class T>
Bitonic<T>::Bitonic(std::string source_dir, EnjaDevice* ed_ )
{
    this->ed = ed_;
    /*
    this->cl_dstkey = dstkey;
    this->cl_dstval = dstval;
    this->cl_srckey = srckey;
    this->cl_srcval = srcval;
    */
    loadKernels(source_dir);
}

template <class T>
void Bitonic<T>::loadKernels(std::string source_dir)
{
    source_dir += "/bitonic.cl";

    std::string options = "-D LOCAL_SIZE_LIMIT=512";
    //cl::Program prog = cli->loadProgram(source_dir, options);
    //k_bitonicSortLocal = Kernel(cli, prog, "bitonicSortLocal");
    k_bitonicSortLocal = Kernel(ed, source_dir, "bitonicSortLocal");
    k_bitonicSortLocal.build(options);
    //k_bitonicSortLocal1 = Kernel(cli, prog, "bitonicSortLocal1");
    k_bitonicSortLocal1 = Kernel(ed, source_dir, "bitonicSortLocal1");
    k_bitonicSortLocal1.build(options); 
    //k_bitonicMergeLocal = Kernel(cli, prog, "bitonicMergeLocal");
    k_bitonicMergeLocal = Kernel(ed, source_dir, "bitonicMergeLocal");
    k_bitonicMergeLocal.build(options); 
    //k_bitonicMergeGlobal = Kernel(cli, prog, "bitonicMergeGlobal");
    k_bitonicMergeGlobal = Kernel(ed, source_dir, "bitonicMergeGlobal");
    k_bitonicMergeGlobal.build(options);

    //TODO: implement this check with the C++ API    
    //if( (szBitonicSortLocal < (LOCAL_SIZE_LIMIT / 2)) || (szBitonicSortLocal1 < (LOCAL_SIZE_LIMIT / 2)) || (szBitonicMergeLocal < (LOCAL_SIZE_LIMIT / 2)) ){
            //shrLog("\nERROR !!! Minimum work-group size %u required by this application is not supported on this device.\n\n", LOCAL_SIZE_LIMIT / 2);

    /*
    printf("bitonic dev pointers: %d\n", cl_dstkey->getBuffer());
    printf("bitonic dev pointers: %d\n", cl_dstval->getBuffer());
    printf("bitonic dev pointers: %d\n", cl_srckey->getBuffer());
    printf("bitonic dev pointers: %d\n", cl_srcval->getBuffer());
    */


}

static cl_uint factorRadix2(cl_uint& log2L, cl_uint L){
    if(!L){
        log2L = 0;
        return 0;
    }else{
        for(log2L = 0; (L & 1) == 0; L >>= 1, log2L++);
        return L;
    }
}



template <class T>
int Bitonic<T>::Sort(int batch, int arrayLength, int dir,
                    Buffer<T> *cl_dstkey, Buffer<T> *cl_dstval, 
                    Buffer<T> *cl_srckey, Buffer<T> *cl_srcval)
{

    if(arrayLength < 2)
        return 0;


    int arg = 0;
    k_bitonicSortLocal.setArg(arg++, cl_dstkey->getBuffer());
    k_bitonicSortLocal.setArg(arg++, cl_dstval->getBuffer());
    k_bitonicSortLocal.setArg(arg++, cl_srckey->getBuffer());
    k_bitonicSortLocal.setArg(arg++, cl_srcval->getBuffer());

    arg = 0;
    k_bitonicSortLocal1.setArg(arg++, cl_dstkey->getBuffer());
    k_bitonicSortLocal1.setArg(arg++, cl_dstval->getBuffer());
    k_bitonicSortLocal1.setArg(arg++, cl_srckey->getBuffer());
    k_bitonicSortLocal1.setArg(arg++, cl_srcval->getBuffer());

    arg = 0;
    k_bitonicMergeGlobal.setArg(arg++, cl_dstkey->getBuffer());
    k_bitonicMergeGlobal.setArg(arg++, cl_dstval->getBuffer());
    k_bitonicMergeGlobal.setArg(arg++, cl_dstkey->getBuffer());
    k_bitonicMergeGlobal.setArg(arg++, cl_dstval->getBuffer());

    arg = 0;
    k_bitonicMergeLocal.setArg(arg++, cl_dstkey->getBuffer());
    k_bitonicMergeLocal.setArg(arg++, cl_dstval->getBuffer());
    k_bitonicMergeLocal.setArg(arg++, cl_dstkey->getBuffer());
    k_bitonicMergeLocal.setArg(arg++, cl_dstval->getBuffer());






    //Only power-of-two array lengths are supported so far
    cl_uint log2L;
    cl_uint factorizationRemainder = factorRadix2(log2L, arrayLength);
    //printf("bitonic factorization remainder: %d\n", factorizationRemainder);
    
    dir = (dir != 0);
    //printf("dir: %d\n", dir);

    int localWorkSize;
    int globalWorkSize;

    if(arrayLength <= LOCAL_SIZE_LIMIT)
    {
         //Launch bitonicSortLocal
        k_bitonicSortLocal.setArg(4, arrayLength);
        k_bitonicSortLocal.setArg(5, dir); 

        localWorkSize  = LOCAL_SIZE_LIMIT / 2;
        globalWorkSize = batch * arrayLength / 2;
        k_bitonicSortLocal.execute(globalWorkSize, localWorkSize);
  
    }
    else
    {
        //Launch bitonicSortLocal1
        
        localWorkSize  = LOCAL_SIZE_LIMIT / 2;
        globalWorkSize = batch * arrayLength / 2;
        //printf("sortlocal size: %d\n", globalWorkSize);
        k_bitonicSortLocal1.execute(globalWorkSize, localWorkSize);

        for(uint size = 2 * LOCAL_SIZE_LIMIT; size <= arrayLength; size <<= 1)
        {
            for(unsigned stride = size / 2; stride > 0; stride >>= 1)
            {
                if(stride >= LOCAL_SIZE_LIMIT)
                {
                    //Launch bitonicMergeGlobal
                    k_bitonicMergeGlobal.setArg(4, arrayLength);
                    k_bitonicMergeGlobal.setArg(5, size);
                    k_bitonicMergeGlobal.setArg(6, stride);
                    k_bitonicMergeGlobal.setArg(7, dir); 

                    //printf("batch: %d arrayLength / 2: %d\n", batch, arrayLength / 2);
                    globalWorkSize = batch * arrayLength / 2;
                    //printf("mergeglobal global size: %d\n", globalWorkSize);
                    k_bitonicMergeGlobal.execute(globalWorkSize);
                }
                else
                {
                    //Launch bitonicMergeLocal
                    
                    
                    k_bitonicMergeLocal.setArg(4, arrayLength);
                    k_bitonicMergeLocal.setArg(5, stride);
                    k_bitonicMergeLocal.setArg(6, size);
                    k_bitonicMergeLocal.setArg(7, dir); 

                    localWorkSize  = LOCAL_SIZE_LIMIT / 2;
                    globalWorkSize = batch * arrayLength / 2;
                    
                    //printf("mergelocal global size: %d\n", globalWorkSize);
                    k_bitonicMergeLocal.execute(globalWorkSize, localWorkSize);
                    break;
                }
                //printf("globalWorkSize: %d\n", globalWorkSize);
            }
        }




        
    }

    return localWorkSize;

    


    //scopy(num, cl_sort_output_hashes.getBuffer(), 
	//             cl_sort_hashes.getBuffer());
	//scopy(num, cl_sort_output_indices.getBuffer(), 
	//             cl_sort_indices.getBuffer());
    


}
