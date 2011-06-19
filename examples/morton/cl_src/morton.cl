
#define oob(X) ( x < 0 && x >= grid.nb_cells )

__kernel void morton(__global float4* pos, __global int* hashes)
{
    int i = get_global_id(0);
#if 0
int hashmin = calcMorton(calcCell(pos[i]) - 1);
int hashmax = calcMorton(calcCell(pos[i]) + 1);
if oob(hashmin)
    hashmin = 0;
if oob(hashmax)
    hashmax = grid.nb_cells;

//todo check for j in bounds
int j = i+1;
hashj = calcMorton(calcCell(pos[j]));
while( hashj < hashmax )
{
    //neighbor stuff

    j++;
    hashj = calcMorton(calcCell(pos[j]));
}

int j = i-1;
hashj = calcMorton(calcCell(pos[j]));
while( hashj < hashmax )
{
    //neighbor stuff

    j++;
    hashj = calcMorton(calcCell(pos[j]));
}
#endif
}
