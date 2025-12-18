#ifndef VT_DISTANCE_H
#define VT_DISTANCE_H
#include "vtraceCommon.h"
#include "vtrace.h"


typedef float (*distance_func) (float* a, float* b, uint32 dim);

extern float l2_sq_distance(float* a, float* b, uint32 dim);

#endif