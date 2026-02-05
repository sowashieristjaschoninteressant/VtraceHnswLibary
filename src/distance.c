#include "distance.h"

#include "vtrace.h"
// shit when i compile with optimisation flags the floating point comparision is dying so i might have a problem here
 long double l2_sq_distance(vec* a, vec* b){
    long double dist = 0.0f;

    for(uint32 i = 0; i < a->dim; i++){

        long double diff = a->vec[i] - b->vec[i];

        dist += diff * diff;

    }

    return dist;
}
