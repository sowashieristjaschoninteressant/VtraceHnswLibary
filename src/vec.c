#include "vec.h"
#include "alloc.h"

vec make_vec(uint32 dim, float32* values) {

    vec v;
    v.vec = hnsw_alloc_mem(sizeof(float32) * dim, alignof(dim));
    HNSW_ASSERT(v.vec);
    v.dim = dim;
    memcpy(v.vec, values, sizeof(float32) * dim);

    return v;

}