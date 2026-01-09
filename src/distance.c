#include "distance.h"

#include "vtrace.h"

 float32 l2_sq_distance(vec* a, vec* b){
    float32 dist = 0.0f;

    for(uint32 i = 0; i < a->dim; i++){

        double diff = a->vec[i] - b->vec[i];

        dist += diff * diff;

    }

    return dist;
}
