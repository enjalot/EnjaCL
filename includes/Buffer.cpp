
#include <stdio.h>

template <class T>
Buffer<T>::Buffer(EnjaDevice* dev, std::vector<T>* data)
{
    this->host_buff = data;
    cl_buffer = new cl::Buffer(cli->context, CL_MEM_READ_WRITE, host_buff.size()*sizeof(T));
    copyToDevice();
}

template <class T>
Buffer<T>::Buffer(EnjaDevice* dev, const std::vector<T>* data, unsigned int memtype)
{
    this->host_buff = data;

    cl_buffer = new cl::Buffer(cli->context, memtype, host_buff.size()*sizeof(T));
    copyToDevice();


}


template <class T>
Buffer<T>::Buffer(EnjaDevice* dev, GLuint bo_id)
{
    cl_buffer = new cl::BufferGL(cli->context, CL_MEM_READ_WRITE, bo_id);
}

template <class T>
Buffer<T>::~Buffer()
{
    delete cl_buffer;
    delete host_buff;
}

template <class T>
void Buffer<T>::acquire(bool blocking)
{
    dev->getQueue().enqueueAcquireGLObjects(cl_buffer, NULL, &event);
}


template <class T>
void Buffer<T>::release(bool blocking)
{
    dev->getQueue().enqueueReleaseGLObjects(cl_buffer, NULL, &event);
}


template <class T>
void Buffer<T>::copyToDevice(bool blocking)
{
    dev->getQueue().enqueueWriteBuffer(cl_buffer, blocking?CL_TRUE:CL_FALSE, 0, host_buff.size()*sizeof(T), &host_buff[0], NULL, &event);
}

template <class T>
void Buffer<T>::copyToDevice(int start, bool blocking)
{
    dev->getQueue().enqueueWriteBuffer(*cl_buffer, blocking?CL_TRUE:CL_FALSE, start*sizeof(T), host_buff.size()*sizeof(T), &host_buff[start], NULL, &event);
}

template <class T>
void Buffer<T>::copyToHost(int num, bool blocking)
{
    dev->getQueue().enqueueReadBuffer(*cl_buffer, blocking?CL_TRUE:CL_FALSE, 0, host_buff.size()*sizeof(T), &host_buff[0], NULL, &event);
    return host_buff;
}

template <class T>
void Buffer<T>::copyToHost(int num, int start, bool blocking)
{
    dev->getQueue().enqueueReadBuffer(*cl_buffer, blocking?CL_TRUE:CL_FALSE, start*sizeof(T), host_buff.size()*sizeof(T), &host_buff[start], NULL, &event);
    return host_buff;
}

template <class T>
void Buffer<T>::copyFromBuffer(Buffer<T> src, size_t start_src, size_t start_dst, size_t size)
{
    if(src.getDevice().getContext()==dev->getContext())
        dev->getQueue().enqueueCopyBuffer(src.getBuffer(), *cl_buffer, start_src*sizeof(T), start_dst*sizeof(T), size*sizeof(T), NULL, &event);
}
