#ifndef VT_DISTANCE_H
#define VT_DISTANCE_H
typedef struct vector vec;

typedef long double (*distance_func) (vec* a, vec* b);

 long double l2_sq_distance(vec* a, vec* b);


 long double cosine_distance(vec* a, vec* b);

#endif