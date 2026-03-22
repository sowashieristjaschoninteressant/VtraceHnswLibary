#ifndef HNSW_VEC_H
#define HNSW_VEC_H
#include "vtraceCommon.h"



struct vector {
  float* vec;
  uint32 dim;
};

typedef struct vector vec;
vec make_vec(uint32 dim, float32* values);

#endif