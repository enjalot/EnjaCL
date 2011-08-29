
#include <stdio.h>

template <class T>
Image<T>::Image(EnjaDevice* dev, size_t size, cl_mem_flags memtype)
{
    this->dev=dev;
    host_buff = new std::vector<T>(size);
    cl_image = new cl::Image(dev->getContext(),memtype, size*sizeof(T));

}

template <class T>
Image<T>::Image(EnjaDevice* dev, std::vector<T>* data, cl_mem_flags memtype)
{
    this->dev = dev;
    this->host_buff = data;
    cl_image = new cl::Image(dev->getContext(), memtype, host_buff->capacity()*sizeof(T));
    copyToDevice();
    
}

template <class T>
Image<T>::Image(EnjaDevice* dev, GLuint vbo_id, cl_mem_flags memtype)
{
    this->dev=dev;
    cl_image = new cl::ImageGL(dev->getContext(), memtype, vbo_id);
    host_buff = new std::vector<T>(cl_image->getInfo<CL_MEM_SIZE>()/sizeof(T));
}

template <class T>
Image<T>::~Image()
{
    delete cl_image;
    delete host_buff;
}

/*template <class T>
Image<T>::Image(const Image<T>& b)
{
    cl_image=b.cl_image;
    host_buff=b.host_buff;
    dev=b.dev;
    event=b.event;
    vbo_id=b.vbo_id;
}

template <class T>
Image<T>& Image<T>::operator=(const Image<T>& b)
{
    cl_image=b.cl_image;
    host_buff=b.host_buff;
    dev=b.dev;
    event=b.event;
    vbo_id=b.vbo_id;
    return *this;
}*/

template <class T>
void Image<T>::acquire()
{
    std::vector<cl::Memory> buf;
    buf.push_back(*cl_image);
    dev->getQueue().enqueueAcquireGLObjects(&buf, NULL, &event);
    //FIXME: Currently required to block because once the function returns
    //buf is nolonger valid causeing a segfault.
    dev->getQueue().flush();
    dev->getQueue().finish();
}


template <class T>
void Image<T>::release()
{
    std::vector<cl::Memory> buf;
    buf.push_back(*cl_image);
    dev->getQueue().enqueueReleaseGLObjects(&buf, NULL, &event);
    //FIXME: Currently required to block because once the function returns
    //buf is nolonger valid causeing a segfault.
    dev->getQueue().flush();
    dev->getQueue().finish();
}


template <class T>
void Image<T>::copyToDevice(bool blocking)
{
    dev->getQueue().enqueueWriteImage(*cl_image, blocking?CL_TRUE:CL_FALSE, 0, host_buff->size()*sizeof(T), &(*host_buff)[0], NULL, &event);
}

template <class T>
void Image<T>::copyToDevice(int start, bool blocking)
{
    size_t offset = start*sizeof(T);
    dev->getQueue().enqueueWriteImage(*cl_image, blocking?CL_TRUE:CL_FALSE, offset , host_buff->size()*sizeof(T)-offset, &(*host_buff)[start], NULL, &event);
}


template <class T>
void Image<T>::copyToHost(int num, bool blocking)
{
    dev->getQueue().enqueueReadImage(*cl_image, blocking?CL_TRUE:CL_FALSE, 0, num==0?host_buff->size()*sizeof(T):num*sizeof(T), &(*host_buff)[0], NULL, &event);
}

template <class T>
void Image<T>::copyToHost(int start, int num, bool blocking)
{
    size_t offset = start*sizeof(T);
    dev->getQueue().enqueueReadImage(*cl_image, blocking?CL_TRUE:CL_FALSE, offset, num==0?(host_buff->size()*sizeof(T)-offset):num*sizeof(T), &(*host_buff)[start], NULL, &event);
}

template <class T>
void Image<T>::copyFromImage(Image<T>& src, size_t start_src, size_t start_dst, size_t size, bool blocking)
{
    //FIXME: I believe the devices have to belong to the same context to call Copy Image on them.
    //if(src.getDevice().getContext()==dev->getContext())
        dev->getQueue().enqueueCopyImage(src.getImage(), *cl_image, start_src*sizeof(T), start_dst*sizeof(T), size*sizeof(T), NULL, &event);
    //else
    //{
    //    dev->getQueue().enqueueReadImage(src.getImage(),blocking?CL_TRUE:CL_FALSE,start_src*sizeof(T),size*sizeof(T),&(*host_buff)[start_dst],NULL,&event);
    //    copyToDevice(start_dst,blocking);
    //}
}

template <class T>
void Image<T>::create(int size, T val)
{
    delete host_buff;
    host_buff = new std::vector<T>(size,val);
}
template <class T>
void Image<T>::attachHostImage(std::vector<T>* buf)
{
    delete host_buff;
    host_buff = buf;
}
template <class T>
std::vector<T>* Image<T>::releaseHostImage()
{
    std::vector<T>* retval = host_buff;
    host_buff = NULL;
    return retval;
}

template <class T>
std::vector<T>& Image<T>::getHostImage()
{
    return *host_buff;
}
