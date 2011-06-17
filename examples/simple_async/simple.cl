__kernel void simple(__global float4* position,
                     int num,   //number of seeds
                     float dt,  //time step
                     float4 vel //velocity
                     )
{
    //get global id of this thread and use as index into position array
    int i = get_global_id(0);
    if(i >= num) return;
    //grab the position for manipulation (sure we could do all this in one
    //line, but in reality you will use p much more)
    float4 p = position[i];
    //Euler integration
    for(int j = 0; j < 10000; j++)
    {
        p += dt * vel;
    }
    //store position back
    position[i] = p;

}
