#include "vtrace.h"



/**
 * @brief 
 * level sampler to create a geometric distribution between our graph layer sparse -> dense
 * @param lMax 
 * @param level_mult 
 * @return int 
 */
int levelSample(uint32 lMax,float level_mult){
    // PUT THIS INTO THE LIBINIT FUNCTION
    srand( (int) time(NULL));
    
    float u = (float)rand() / ((float) RAND_MAX + 1.0f);

    if(u == 0.0f) u = 1e-10f;

    uint32 level = (int) (-logf(u) * level_mult);

    return level > lMax ? lMax : level;
}