#include "distance.h"


HNSW_INLINE float32 l2_sq_distance(float* a, float* b, uint32 dim){
    float32 dist = 0.0f;

    for(uint32 i = 0; i < dim; i++){

        double diff = a[i] - b[i];

        dist += diff * diff;

    }

    return dist;
}
