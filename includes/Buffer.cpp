
#include <stdio.h>

template <class T>
Buffer<T>::Buffer(EnjaDevice* dev, size_t size, cl_mem_flags memtype)
{
    this->dev=dev;
    host_buff = new vector<T>(size);
    cl_buffer = new cl::Buffer(dev->getContext(), memtype, host_buff.size()*sizeof(T));
}

template <class T>
Buffer<T>::Buffer(EnjaDevice* dev, std::vector<T>* data, cl_mem_flags memtype)
{
    this->dev = dev;
    this->host_buff = data;

    cl_buffer = new cl::Buffer(dev->getContext(), memtype, host_buff.size()*sizeof(T));
    copyToDevice();
}

template <class T>
Buffer<T>::Buffer(EnjaDevice* dev, GLuint bo_id, cl_mem_flags memtype)
{
    cl_buffer = new cl::BufferGL(dev->getContext(), memtype, bo_id);
    host_buff = new vector<T>(cl_buffer->getInfo<CL_MEM_SIZE>());
}

template <class T>
Buffer<T>::~Buffer()
{
    delete cl_buffer;
    delete host_buff;
}

template <class T>
void Buffer<T>::acquire()
{
    dev->getQueue().enqueueAcquireGLObjects(cl_buffer, NULL, &event);
}


template <class T>
void Buffer<T>::release()
{
    dev->getQueue().enqueueReleaseGLObjects(cl_buffer, NULL, &event);
}


template <class T>
void Buffer<T>::copyToDevice(bool blocking)
{
    dev->getQueue().enqueueWriteBuffer(cl_buffer, blocking?CL_TRUE:CL_FALSE, 0, host_buff.size()*sizeof(T), &(*host_buff)[0], NULL, &event);
}

template <class T>
void Buffer<T>::copyToDevice(int start, bool blocking)
{
    size_t offset = start*sizeof(T);
    dev->getQueue().enqueueWriteBuffer(*cl_buffer, blocking?CL_TRUE:CL_FALSE, offset , host_buff.size()*sizeof(T)-offset, &(*host_buff)[start], NULL, &event);
}

template <class T>
void Buffer<T>::copyToHost(int num, bool blocking)
{
    dev->getQueue().enqueueReadBuffer(*cl_buffer, blocking?CL_TRUE:CL_FALSE, 0, host_buff.size()*sizeof(T), &(*host_buff)[0], NULL, &event);
}

template <class T>
void Buffer<T>::copyToHost(int num, int start, bool blocking)
{
    size_t offset = start*sizeof(T);
    dev->getQueue().enqueueReadBuffer(*cl_buffer, blocking?CL_TRUE:CL_FALSE, offset, host_buff.size()*sizeof(T)-offset, &(*host_buff)[start], NULL, &event);
}

template <class T>
void Buffer<T>::copyFromBuffer(Buffer<T> src, size_t start_src, size_t start_dst, size_t size, bool blocking)
{
    if(src.getDevice().getContext()==dev->getContext())
        dev->getQueue().enqueueCopyBuffer(src.getBuffer(), *cl_buffer, start_src*sizeof(T), start_dst*sizeof(T), size*sizeof(T), NULL, &event);
    else
    {
        dev->getQueue().enqueueReadBuffer(src.getBuffer(),blocking?CL_TRUE:CL_FALSE,start_src*sizeof(T),size*sizeof(T),&(*host_buff)[start_dst],NULL,&event);
        copyToDevice(start_dst,blocking);
    }
}

template <class T>
void create(int size, T val)
{
    delete host_buff;
    host_buff = new vector<T>(size,val);
}
template <class T>
void attachHostBuffer(std::vector<T>* buf)
{
    delete host_buff;
    host_buff = buf;
}
template <class T>
std::vector<T>* releaseHostBuffer()
{
    std::vector<T>* retval = host_buff;
    host_buff = NULL;
    return retval;
}
template <class T>
std::vector<T>* const getHostBuffer()
{
    return host_buff;
}
