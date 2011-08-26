/* 
 * File:   main_multigpu_gl.cpp
 * Author: asy10
 *
 * Created on August 11, 2011, 1:08 PM
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>

#include <GL/glew.h>
#if defined __APPLE__ || defined(MACOSX)
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
//OpenCL stuff
#endif

#include <CLL.h>
#include <EnjaDevice.h>
#include <Buffer.h>
#include <Kernel.h>
#include "timer_eb.h"
#include <util.h>
#include <structs.h>

using namespace std;
using namespace enjacl;
using namespace EB;

const int NDRANGE = 1;

CL cli(true);
//vector<EnjaDevice>* devs = NULL;
Kernel* kernels = NULL;
Buffer<float4>* pos = NULL;
GLuint posVBO = 0;
GLuint colVBO = 0;
size_t size_per_card = 0;

const float4 COLORS[] = {float4(1.0f,0.0f,0.0f,1.0f),float4(0.0f,1.0f,0.0f,1.0f),float4(0.0f,0.0f,1.0f,1.0f),float4(0.5f,0.5f,0.0f,1.0f),float4(0.5f,0.0f,0.5f,1.0f),float4(0.0f,0.5f,0.5f,1.0f)};

float4 dPos4f(0.002f, 0.003f, 0.0f,0.0f);
//Buffer<float4>** dPos=NULL;

int vector_size = 256;
int window_width = 800;
int window_height = 600;
int glutWindowHandle = 0;

float translate_x = -2.00f;
float translate_y = -2.70f;//300.f;
float translate_z = 3.50f;

// mouse controls
int mouse_old_x, mouse_old_y;
int mouse_buttons = 0;
float rotate_x = 0.0, rotate_y = 0.0;

const string kernel_str = "\n__kernel void vect_add(__global float4* a, __global float4* dPos)\n"
                           "{\n"
                           "    int index = get_global_id(0);\n"
                           "    a[index] += dPos[0];\n"
                           "}\n";

//timers
//GE::Time *ts[3];

void printFloatVector(vector<float>& vec)
{
    printf("[");
    for(int i = 0; i<vec.size(); i++)
    {
        printf("%f,",vec[i]);
    }
    printf("\b]\n");
}

void init_gl()
{
    // default initialization
    //glDisable(GL_DEPTH_TEST);

    // viewport
    glViewport(0, 0, window_width, window_height);

    // projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(fovy, aspect, nearZ, farZ);
    gluPerspective(60.0, (GLfloat)window_width / (GLfloat) window_height, 0.1, 100.0);
    //gluPerspective(fov, (GLfloat)window_width / (GLfloat) window_height, 0.3, 100.0);
    //gluPerspective(90.0, (GLfloat)window_width / (GLfloat) window_height, 0.1, 10000.0); //for lorentz

    // set view matrix
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(.2, .2, .6, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //ps->system->getRenderer()->setWindowDimensions(window_width,window_height);
    //glTranslatef(0, 10, 0);
    /*
    gluLookAt(  0,10,0,
                0,0,0,
                0,0,1);
    */


    //glTranslatef(0, translate_z, translate_y);
    //glRotatef(-90, 1.0, 0.0, 0.0);

    return;

}

void appMouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        mouse_buttons |= 1<<button;
    }
    else if (state == GLUT_UP)
    {
        mouse_buttons = 0;
    }

    mouse_old_x = x;
    mouse_old_y = y;

    //glutPostRedisplay();
}

void appMotion(int x, int y)
{
    float dx, dy;
    dx = x - mouse_old_x;
    dy = y - mouse_old_y;

    if (mouse_buttons & 1)
    {
        rotate_x += dy * 0.2;
        rotate_y += dx * 0.2;
    }
    else if (mouse_buttons & 4)
    {
        translate_z -= dy * 0.1;
    }

    mouse_old_x = x;
    mouse_old_y = y;

    // set view matrix
    glutPostRedisplay();
}

void resizeWindow(int w, int h)
{
    if (h==0)
    {
        h=1;
    }
    glViewport(0, 0, w, h);

    // projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)window_width / (GLfloat) window_height, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //gluPerspective(fov, aspect, nearZ, farZ);
    //ps->system->getRenderer()->setWindowDimensions(w,h);
    window_width = w;
    window_height = h;
    //delete[] image;
    //image = new GLubyte[w*h*4];
    //setFrustum();
    glutPostRedisplay();
}

void appDestroy()
{

    if (glutWindowHandle)glutDestroyWindow(glutWindowHandle);
    printf("about to exit!\n");

    exit(0);
}

void appKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'p': //print timers
            return;
        case '\033': // escape quits
        case '\015': // Enter quits    
        case 'Q':    // Q quits
        case 'q':    // q (or escape) quits
            // Cleanup up and quit
            appDestroy();
            return;
        default:
            return;
    }

    glutPostRedisplay();
}

void timerCB(int ms)
{
    glutTimerFunc(ms, timerCB, ms);
    debugf("%s","here");
    glFinish();
    debugf("%s","here");
    vector<EnjaDevice>& devs = cli[CL_DEVICE_TYPE_GPU];
    try
    {
    pos[0].acquire();
    //devs->at(0).getQueue().finish();
    #pragma parallel for
    for(int i = 0; i<devs.size();i++)
    {
        kernels->setArg(0,pos[i].getBuffer());
        kernels->setArg(1,dPos4f);
        kernels->execute(vector_size/devs.size());
        devs[i].getQueue().flush();
        devs[i].getQueue().finish();
    }
    
    #pragma parallel for
    for(int i = 1; i<devs.size(); i++)
    {
        pos[0].copyFromBuffer(pos[i],0, size_per_card*i, size_per_card);
    }
    devs[0].getQueue().flush();
    devs[0].getQueue().finish();
    
    pos[0].release();
    }
    catch(cl::Error er)
    {
        printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
    }
//    ps->update();
    glutPostRedisplay();
}

void appRender()
{

    glEnable(GL_DEPTH_TEST);

    // set view matrix
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBindBuffer(GL_ARRAY_BUFFER, colVBO);
    glColorPointer(4, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, posVBO);
    glVertexPointer(4, GL_FLOAT, 0, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glColor4f(1.0f, 0.0f, .5f, 1.0f);
    //Need to disable these for blender
    //glDisableClientState(GL_NORMAL_ARRAY);
    glDrawArrays(GL_POINTS, 0, pos[0].size());

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    //glRotatef(-90, 1.0, 0.0, 0.0);
    //glRotatef(rotate_x, 1.0, 0.0, 0.0);
    //glRotatef(rotate_y, 0.0, 0.0, 1.0); //we switched around the axis so make this rotate_z
    //glTranslatef(translate_x, translate_z, translate_y);
    /*
    glBegin(GL_TRIANGLES);
        glVertex3f(1.0f,0.0f,-5.0f);
        glVertex3f(1.0f,1.0f,-5.0f);
        glVertex3f(0.0f,0.5f,-5.0f);
    glEnd();
    */
    glutSwapBuffers();

    //glDisable(GL_DEPTH_TEST);
}

class CLProfiler
{
public:
    void addEvent(const char* name, int device_num, int num_dev, cl::Event& event)
    {
        cl_ulong start, end, queued, submit;
        event.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
        event.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
        event.getProfilingInfo(CL_PROFILING_COMMAND_QUEUED, &queued);
        event.getProfilingInfo(CL_PROFILING_COMMAND_SUBMIT, &submit);
        double timing = (end - start) * 1.0e-6; 
        stringstream s;
        s<<name<< " # "<<device_num+1<<" of "<<num_dev;
        cl_timing& tmp = timings[s.str()]; 
        tmp.end_time = end * 1.0e-6;
        tmp.start_time = start * 1.0e-6;
        tmp.queue_time = queued * 1.0e-6;
        tmp.submit_time = submit * 1.0e-6;
        if(timing>tmp.max_time)
            tmp.max_time = timing;
        if(timing<tmp.min_time)
            tmp.min_time = timing;
        tmp.total_time += timing;
        tmp.num_times += 1;
    }
    void printAll()
    {
        for (map<string, cl_timing>::iterator i = timings.begin();
              i!=timings.end(); i++)
        {
            cout<<i->first<<":"<<endl;
            /*cout<<"\tMinimum Time:\t\t"<<*min_element(i->second.begin(),i->second.end())<<endl;
            cout<<"\tMaximum Time:\t\t"<<*max_element(i->second.begin(),i->second.end())<<endl;
            float total = 0.0;
            for(vector<float>::iterator j = i->second.begin(); j!=i->second.end(); j++)
                total+=*j;
            cout<<"\tAverage Time:\t\t"<<total/i->second.size()<<endl;
            cout<<"\tTotal Time:\t\t"<<total<<endl;
            cout<<"\tCount:\t\t"<<i->second.size()<<"\n"<<endl;**/
            cout<<setprecision(15)<<fixed;
            cout<<"\tSubmit Time:\t\t"<<i->second.submit_time<<endl;
            cout<<"\tQueue Time:\t\t"<<i->second.queue_time<<endl;
            cout<<"\tStart Time:\t\t"<<i->second.start_time<<endl;
            cout<<"\tEnd Time:\t\t"<<i->second.end_time<<endl;
            cout<<"\tMinimum Time:\t\t"<<i->second.min_time<<endl;
            cout<<"\tMaximum Time:\t\t"<<i->second.max_time<<endl;
            cout<<"\tAverage Time:\t\t"<<i->second.total_time/i->second.num_times<<endl;
            cout<<"\tTotal Time:\t\t"<<i->second.total_time<<endl;
            cout<<"\tCount:\t\t"<<i->second.num_times<<"\n"<<endl;
        }
    }
private:
    struct cl_timing
    {
        cl_timing()
        {
            min_time = numeric_limits<double>::max();
            max_time = numeric_limits<double>::min();
            total_time = start_time = end_time = queue_time = submit_time = 0.0;
            num_times = 0;
        }
        double min_time;
        double max_time;
        double total_time;
        double queue_time;
        double submit_time;
        double start_time;
        double end_time;
        int num_times;
    };
    map<string, cl_timing > timings;
};

void printEventInfo(const char* name, int device_num, cl::Event& event,map<string,vector<float> >& )
{
    
}
//================
//#include "materials_lights.h"

//----------------------------------------------------------------------
float rand_float(float mn, float mx)
{
    float r = rand() / (float) RAND_MAX;
    return mn + (mx-mn)*r;
}
//----------------------------------------------------------------------


//----------------------------------------------------------------------
int main(int argc, char** argv)
{
    //int num_runs = 1;

    //initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH
                //|GLUT_STEREO //if you want stereo you must uncomment this.
                );
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition (glutGet(GLUT_SCREEN_WIDTH)/2 - window_width/2,
                            glutGet(GLUT_SCREEN_HEIGHT)/2 - window_height/2);

    glutWindowHandle = glutCreateWindow("Simple MultiGPU - OpenCL/OpenGL");

    glutDisplayFunc(appRender); //main rendering function
    glutTimerFunc(30, timerCB, 30); //determine a minimum time between frames
    glutKeyboardFunc(appKeyboard);
    glutMouseFunc(appMouse);
    glutMotionFunc(appMotion);
    glutReshapeFunc(resizeWindow);

    glewInit();
    GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0 GL_ARB_pixel_buffer_object");
    debugf("GLEW supported?: %d\n", bGLEW);

    init_gl();

    CLProfiler prof;
    //Argument 1 sets the size of vectors
    if(argc>1)
    {
        vector_size = atoi(argv[1]);
    }
    vector_size=nlpo2(vector_size);

    //Argument 2 sets the number of times to run
    /*if(argc>2)
    {
        num_runs = atoi(argv[2]);
    }*/

    printf("Vector size is %d\n",vector_size);
    //printf("Number of times to run %d\n",num_runs);

    //cli = CL(true);
    vector<EnjaDevice>& devs = cli[CL_DEVICE_TYPE_GPU];
    kernels = new Kernel[devs.size()];
    for(int i=0; i<devs.size(); i++)
    {
        kernels[i].setEnjaDevice(&devs[i]);
        kernels[i].setName("vect_add");
        kernels[i].buildFromStr(kernel_str);
    }
    //clGetGLContextInfoKHR()


    
    
    vector<float4>* vec = new vector<float4>(vector_size);
    vector<float4>* col = new vector<float4>(vector_size); 
    for(int i = 0; i < vector_size; i++)
    {
        (*vec)[i]=float4(rand_float(-1.0,1.0),rand_float(-1.0,1.0),rand_float(-5.0,-6.0),1.0);
    }

    
    size_per_card = vector_size/devs.size();
    for(int i = 0,cur_dev = 0; i <vector_size; i++)
    {
        if(i%size_per_card==0 && i!=0)
            cur_dev++;
        (*col)[i]=COLORS[cur_dev];
    }
    try
    {
        posVBO = createVBO((void*)&((*vec)[0]),vec->size()*sizeof(float4),GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW);
        colVBO = createVBO((void*)&((*col)[0]),col->size()*sizeof(float4),GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW);
        
	debugf("posVBO = %d",posVBO);
	debugf("vec->size() = %d",vec->size());

        debugf("%s", "here");
        pos = new Buffer<float4>[devs.size()]; 
        //dPos = new Buffer<float4>[devs.size()];
        /*#pragma parallel for
        for(int i = 0; i < devs->size(); i++)
        {
            dPos[i] = new Buffer<float4>(&(*devs)[i],(size_t)1,CL_MEM_READ_ONLY);
            dPos[i]->getHostBuffer()[0] = dPos4f;
            dPos[i]->copyToDevice();
        }*/
        debugf("%s", "here");

        pos[0] = Buffer<float4>(&devs[0],posVBO);
debugf("&pos[0] = 0x%08x",&pos[0]);
        #pragma parallel for
        for(int i = 1; i < devs.size(); i++)
        {
        
            pos[i] = Buffer<float4>(&devs[i],size_per_card);
            memcpy(&(pos[i][0]),&((*vec)[size_per_card*i]),sizeof(float4)*size_per_card);
            pos[i].copyToDevice();
        }
        debugf("%s", "here");

        #pragma parallel for
	for(int i = 0; i < devs.size(); i++)
	{
            devs[i].getQueue().flush();
            devs[i].getQueue().finish();
	}
        debugf("%s", "here");
    }
    catch (cl::Error er)
    {
        printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
    }
    delete vec;
    delete col;
    //cl::Program progs[cli->getContexts().size()];
    /*vector<EnjaDevice>& devs = cli->getEnjaDevices(CL_DEVICE_TYPE_GPU);
    //vector<EnjaDevice>& devs = cli->getEnjaDevices(CL_DEVICE_TYPE_CPU);
    Kernel kerns[devs.size()];
    
    //Create timers to keep up with execution/memory transfer times.
    int num_timers = 3;

    EB::TimerList timers;
    timers["vect_add_cpu"] = new EB::Timer("Adding vectors on single CPU", 0);
    const char* timer_name_temp[] = {"buffer_write_%d_%ss","vect_add_%d_%ss","buffer_read_%d_%ss"};
    const char* timer_desc_temp[] = {"Writing buffers to %d %ss","Adding vectors on %d %ss","Reading buffers from %d %ss"};

    char timer_name[num_timers*cli->getDevices().size()][256];
    for(int num_gpus = 1; num_gpus<=cli->getDevices().size(); num_gpus++)
    {
        for(int i = 0;i<num_timers; i++)
        {
            char timer_desc[256];
            sprintf(timer_name[i+(num_gpus-1)*num_timers],timer_name_temp[i],num_gpus,"GPU");
            sprintf(timer_desc,timer_desc_temp[i],num_gpus,"GPU");
            timers[timer_name[i+(num_gpus-1)*num_timers]] = new EB::Timer(timer_desc,0);
        }
    }

    //run simulation num_runs times to make sure we get a good statistical sampleing of run times.
    for(int j = 0; j<num_runs; j++)
    {
        int i;
        cout<<"run: "<<j+1<<" of "<< num_runs<<endl;
        
        //Run on 1 gpu, 2 gpus, ..., n gpus where n is the number of devices belonging to the cl context.
        for(int num_gpus = 1; num_gpus<=devs.size(); num_gpus++)
        {
            int timer_num = 3*(num_gpus-1);
            //cl::Event event_a[num_gpus],event_b[num_gpus],event_execute[num_gpus],event_read[num_gpus];

            //Create Buffers for each gpu.
            Buffer<float>* a[num_gpus];
            Buffer<float>* b[num_gpus];
            Buffer<float>* c[num_gpus];
            try
            {
                //Set size and buffer properties for each of the buffer. Divide by num_gpus to evenly distribute
                //data across them
                #pragma omp parallel for private(i) schedule(static,1)
                for(i = 0; i<num_gpus; i++)
                {
                    size_t tmp_size = vector_size/num_gpus;
                    a[i] = new Buffer<float>(&devs[i],tmp_size,CL_MEM_READ_ONLY);
                    b[i] = new Buffer<float>(&devs[i],tmp_size,CL_MEM_READ_ONLY);
                    c[i] = new Buffer<float>(&devs[i],tmp_size,CL_MEM_WRITE_ONLY);
                    for(int k = 0; k<tmp_size; k++)
                    {
                       int ind = k+(i*tmp_size);
                       a[i]->getHostBuffer()->at(k)=ind; 
                       b[i]->getHostBuffer()->at(k)=ind; 
                    }
//                    printFloatVector(*a[i]->getHostBuffer());
//                    printFloatVector(*b[i]->getHostBuffer());
                }

                //Transfer our host buffers to each GPU then wait for it to finish before executing the kernel.
                timers[timer_name[timer_num]]->start();
                #pragma omp parallel for private(i)
                for(i = 0; i<num_gpus; i++)
                {
                    a[i]->copyToDevice();
                    b[i]->copyToDevice();
                    devs[i].getQueue().flush();
                    devs[i].getQueue().finish();
                }
                timers[timer_name[timer_num]]->stop();
                //set the kernel arguments
                for(i=0;i<num_gpus; i++)
                {
                    kerns[i].setArg(0,a[i]->getBuffer());
                    kerns[i].setArg(1,b[i]->getBuffer());
                    kerns[i].setArg(2,c[i]->getBuffer());
                }
                //Set the kernel arguments vec a,b,c and enqueue kernel.
                timers[timer_name[timer_num+1]]->start();
                #pragma omp parallel for private(i) schedule(static,1)
                for(i = 0; i<num_gpus; i++)
                {
                    //FIXME: Need to handle context better/kernels better.
                    //cli->getQueues()[i].enqueueNDRangeKernel(kerns[0].getKernel(),cl::NullRange,  cl::NDRange(vector_size/num_gpus),cl::NullRange , NULL, &event_execute[i]);
                    kerns[i].execute(vector_size/num_gpus);
                    devs[i].getQueue().flush();
                    devs[i].getQueue().finish();
                }
                timers[timer_name[timer_num+1]]->stop();
                
                timers[timer_name[timer_num+2]]->start();
                #pragma omp parallel for private(i)
                for(i = 0; i<num_gpus; i++)
                {
                    //cli->getQueues()[i].enqueueReadBuffer(c_d[i], CL_FALSE, 0, (c_h.size()/num_gpus)*sizeof(float), &c_h[i*(c_h.size()/num_gpus)], NULL, &event_read[i]);
                    c[i]->copyToHost(0,true);
                    devs[i].getQueue().flush();
                    devs[i].getQueue().finish();
                }
                timers[timer_name[timer_num+2]]->stop();
//                for(i = 0; i<num_gpus; i++)
//                {
//                        printFloatVector(*c[i]->getHostBuffer());
//                }
                //add event timings from openCL to our profiler
                for(i = 0; i<num_gpus; i++)
                {
                    prof.addEvent("GPU write buffer a. GPU ",i,num_gpus,a[i]->getEvent());
                    prof.addEvent("GPU write buffer b. GPU ",i,num_gpus,b[i]->getEvent());
                    prof.addEvent("GPU execute vector add. GPU ",i,num_gpus,kerns[i].getEvent());
                    prof.addEvent("GPU read buffer. GPU ",i,num_gpus,c[i]->getEvent());
                }
                
                for(i = 0; i<num_gpus; i++)
                {
                    delete a[i];
                    delete b[i];
                    delete c[i];
                }
            }
            catch (cl::Error er)
            {
                printf("j = %d, num_gpus = %d, i = %d\n",j,num_gpus,i);
                printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
            }
        }
    }*/

    //timers.printAll();
    prof.printAll();

    glutMainLoop();

    /*for(int i = 0; i<devs->size(); i++)
    {
       delete pos[i]; 
       delete dPos[i];
    }**/
    delete[] pos;
    //delete[] dPos;

    //delete cli;
    return 0;
}
