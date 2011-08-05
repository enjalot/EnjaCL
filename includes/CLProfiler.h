/* 
 * File:   CLProfiler.h
 * Author: asy10
 *
 * Created on August 3, 2011, 10:51 AM
 */

#ifndef CLPROFILER_H
#define	CLPROFILER_H

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <vector>
#include <util.h>
    void CL_CALLBACK addEvent(cl_event event, cl_int status, void* user)
    {
        cl_ulong start, end, queued, submit;
//        event.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
//        event.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
//        event.getProfilingInfo(CL_PROFILING_COMMAND_QUEUED, &queued);
//        event.getProfilingInfo(CL_PROFILING_COMMAND_SUBMIT, &submit);
        clGetEventProfilingInfo(event,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&end,NULL);
        clGetEventProfilingInfo(event,CL_PROFILING_COMMAND_START,sizeof(cl_ulong), &start,NULL);
        clGetEventProfilingInfo(event,CL_PROFILING_COMMAND_QUEUED,sizeof(cl_ulong), &queued,NULL);
        clGetEventProfilingInfo(event,CL_PROFILING_COMMAND_SUBMIT,sizeof(cl_ulong), &submit,NULL);
        enjacl::debugf("end = %d start = %d queued = %d submit = %d",end,start,queued,submit);
//        double timing = (end - start) * 1.0e-6; 
//        std::stringstream s;
//        s<<name<< " # "<<device_num+1<<" of "<<num_dev;
//        cl_timing tmp;// = timings[s.str()]; 
//        tmp.end_time = end * 1.0e-6;
//        tmp.start_time = start * 1.0e-6;
//        tmp.queue_time = queued * 1.0e-6;
//        tmp.submit_time = submit * 1.0e-6;
//        timings.push_back(tmp);
//        if(timing>tmp.max_time)
//            tmp.max_time = timing;
//        if(timing<tmp.min_time)
//            tmp.min_time = timing;
//        tmp.total_time += timing;
//        tmp.num_times += 1;
    }
namespace enjacl
{
    class CLProfiler
    {
    public:
        void setName(const std::string& n)
        {
            name = n;
        }
        const std::string getName()
        {
            return name;
        }
//        void addEvent(cl_event event, cl_int status, void* userData);
        void printAll() const;

    private:
        struct cl_timing
        {
            cl_timing()
            {
               start_time = end_time = queue_time = submit_time = 0.0;
            }
            double queue_time;
            double submit_time;
            double start_time;
            double end_time;
        };
        std::vector<cl_timing > timings;
        std::string name;
    };
}

#endif	/* CLPROFILER_H */

