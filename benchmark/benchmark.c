

#include "stdlib.h"
#include "time.h"
#include "stdio.h"
#include "hnsw_public.h"
#include "vtraceCommon.h"
#include "vec.h"

struct timer{
    struct timespec start;
    struct timespec end;
    double elapsed;
};

void timer_start(struct timer* time){
    clock_gettime(CLOCK_MONOTONIC,&time->start);
}

void timer_stop(struct timer* time){
    clock_gettime(CLOCK_MONOTONIC, &time->end);
    time->elapsed = (time->end.tv_sec - time->start.tv_sec) + (time->end.tv_nsec - time->start.tv_nsec)/1e9;
}

void showTime(struct timer* time){
    printf("Time taken: %.6f seconds\n",time->elapsed);
}

static inline float32 randf()
{
    return (float)rand() / (float)RAND_MAX;
}

static inline float32 getRandFloat(float32 min, float32 max)
{
    return min + (max - min) * randf();
}

// generate randome float array
float32 *generateRandVec(uint32 size, float32 min, float32 max)
{

    float32 *farray = malloc(sizeof(float32) * size);
    assert(farray);

    for (uint32 i = 0; i < size; i++)
    {
        farray[i] = getRandFloat(min, max);
    }

    return farray;
}

int main(){

    struct timer time;
    const int testCount = 1000000;
    const int dim = 5;
    HNSW* hnsw = hnsw_init(100);
    vec* testVecs  = malloc(sizeof(struct vector) * testCount);
    assert(testVecs);

    for(int i = 0; i < testCount; i++){

        float32* value = generateRandVec(5, 0, 10000);
        testVecs[i] = (vec) {value, dim};
    }


    HNSW_LOG("START TESTING");
    timer_start(&time);

    for(int i = 0; i < testCount; i++){
        hnsw_insert(hnsw, &testVecs[i],20);
    }

    timer_stop(&time);
    HNSW_LOG("END TEST");
    showTime(&time);


}