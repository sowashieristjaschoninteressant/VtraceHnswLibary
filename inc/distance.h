#ifndef VT_DISTANCE_H
#define VT_DISTANCE_H
#include "vtraceCommon.h"
#include "vtrace.h"
typedef float (*distance_func) (vec* a, vec* b);

HNSW_INLINE float l2_sq_distance(vec* a, vec* b);

#endif