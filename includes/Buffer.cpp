
#include <stdio.h>

template <class T>
Buffer<T>::Buffer(EnjaDevice* dev, size_t size, cl_mem_flags memtype)
{
    this->dev=dev;
    host_buff = new std::vector<T>(size);
    cl_buffer = new cl::Buffer(dev->getContext(),memtype, size*sizeof(T));
}

template <class T>
Buffer<T>::Buffer(EnjaDevice* dev, std::vector<T>* data, cl_mem_flags memtype)
{
    this->dev = dev;
    this->host_buff = data;

    cl_buffer = new cl::Buffer(dev->getContext(), memtype, host_buff->capacity()*sizeof(T));
    copyToDevice();
}

template <class T>
Buffer<T>::Buffer(EnjaDevice* dev, GLuint vbo_id, cl_mem_flags memtype)
{
    cl_buffer = new cl::BufferGL(dev->getContext(), memtype, vbo_id);
    host_buff = new std::vector<T>(cl_buffer->getInfo<CL_MEM_SIZE>());
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
    std::vector<cl::Memory> buf;
    buf.push_back(*cl_buffer);
    dev->getQueue().enqueueAcquireGLObjects(&buf, NULL, &event);
}


template <class T>
void Buffer<T>::release()
{
    std::vector<cl::Memory> buf;
    buf.push_back(*cl_buffer);
    dev->getQueue().enqueueReleaseGLObjects(&buf, NULL, &event);
}


template <class T>
void Buffer<T>::copyToDevice(bool blocking)
{
    dev->getQueue().enqueueWriteBuffer(*cl_buffer, blocking?CL_TRUE:CL_FALSE, 0, host_buff->size()*sizeof(T), &(*host_buff)[0], NULL, &event);
}

template <class T>
void Buffer<T>::copyToDevice(int start, bool blocking)
{
    size_t offset = start*sizeof(T);
    dev->getQueue().enqueueWriteBuffer(*cl_buffer, blocking?CL_TRUE:CL_FALSE, offset , host_buff->size()*sizeof(T)-offset, &(*host_buff)[start], NULL, &event);
}


template <class T>
void Buffer<T>::copyToHost(int num, bool blocking)
{
    dev->getQueue().enqueueReadBuffer(*cl_buffer, blocking?CL_TRUE:CL_FALSE, 0, num==0?host_buff->size()*sizeof(T):num*sizeof(T), &(*host_buff)[0], NULL, &event);
}

template <class T>
void Buffer<T>::copyToHost(int start, int num, bool blocking)
{
    size_t offset = start*sizeof(T);
    dev->getQueue().enqueueReadBuffer(*cl_buffer, blocking?CL_TRUE:CL_FALSE, offset, num==0?(host_buff->size()*sizeof(T)-offset):num*sizeof(T), &(*host_buff)[start], NULL, &event);
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
void Buffer<T>::create(int size, T val)
{
    delete host_buff;
    host_buff = new std::vector<T>(size,val);
}
template <class T>
void Buffer<T>::attachHostBuffer(std::vector<T>* buf)
{
    delete host_buff;
    host_buff = buf;
}
template <class T>
std::vector<T>* Buffer<T>::releaseHostBuffer()
{
    std::vector<T>* retval = host_buff;
    host_buff = NULL;
    return retval;
}
template <class T>
std::vector<T>* const Buffer<T>::getHostBuffer()
{
    return host_buff;
}
