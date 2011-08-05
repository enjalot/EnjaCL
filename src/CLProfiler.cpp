#include <CLProfiler.h>
#include <iostream>
#include <iomanip>

namespace enjacl
{

//    void CLProfiler::addEvent(cl_event event, cl_int status, void* user)
//    {
//        cl_ulong start, end, queued, submit;
////        event.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
////        event.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
////        event.getProfilingInfo(CL_PROFILING_COMMAND_QUEUED, &queued);
////        event.getProfilingInfo(CL_PROFILING_COMMAND_SUBMIT, &submit);
//        clGetEventProfilingInfo(event,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&end,NULL);
//        clGetEventProfilingInfo(event,CL_PROFILING_COMMAND_START,sizeof(cl_ulong), &start,NULL);
//        clGetEventProfilingInfo(event,CL_PROFILING_COMMAND_QUEUED,sizeof(cl_ulong), &queued,NULL);
//        clGetEventProfilingInfo(event,CL_PROFILING_COMMAND_SUBMIT,sizeof(cl_ulong), &submit,NULL);
////        double timing = (end - start) * 1.0e-6; 
////        std::stringstream s;
////        s<<name<< " # "<<device_num+1<<" of "<<num_dev;
//        cl_timing tmp;// = timings[s.str()]; 
//        tmp.end_time = end * 1.0e-6;
//        tmp.start_time = start * 1.0e-6;
//        tmp.queue_time = queued * 1.0e-6;
//        tmp.submit_time = submit * 1.0e-6;
//        timings.push_back(tmp);
////        if(timing>tmp.max_time)
////            tmp.max_time = timing;
////        if(timing<tmp.min_time)
////            tmp.min_time = timing;
////        tmp.total_time += timing;
////        tmp.num_times += 1;
//    }
    
    void CLProfiler::printAll() const
    {
        for (int i = 0; i<timings.size(); i++)//std::vector<cl_timing>::iterator i = timings.begin();
              //i!=timings.end(); i++)
        {
//            std::cout<<i->first<<":"<<std::endl;
            /*cout<<"\tMinimum Time:\t\t"<<*min_element(i->second.begin(),i->second.end())<<endl;
            cout<<"\tMaximum Time:\t\t"<<*max_element(i->second.begin(),i->second.end())<<endl;
            float total = 0.0;
            for(vector<float>::iterator j = i->second.begin(); j!=i->second.end(); j++)
                total+=*j;
            cout<<"\tAverage Time:\t\t"<<total/i->second.size()<<endl;
            cout<<"\tTotal Time:\t\t"<<total<<endl;
            cout<<"\tCount:\t\t"<<i->second.size()<<"\n"<<endl;**/
            std::cout<<std::setprecision(15)<<std::ios_base::fixed;
            std::cout<<"\tSubmit Time:\t\t"<<timings[i].submit_time<<std::endl;
            std::cout<<"\tQueue Time:\t\t"<<timings[i].queue_time<<std::endl;
            std::cout<<"\tStart Time:\t\t"<<timings[i].start_time<<std::endl;
            std::cout<<"\tEnd Time:\t\t"<<timings[i].end_time<<std::endl;
//            std::cout<<"\tMinimum Time:\t\t"<<i->second.min_time<<std::endl;
//            std::cout<<"\tMaximum Time:\t\t"<<i->second.max_time<<std::endl;
//            std::cout<<"\tAverage Time:\t\t"<<i->second.total_time/i->second.num_times<<std::endl;
//            std::cout<<"\tTotal Time:\t\t"<<i->second.total_time<<std::endl;
//            std::cout<<"\tCount:\t\t"<<i->second.num_times<<"\n"<<std::endl;
        }
    }
}