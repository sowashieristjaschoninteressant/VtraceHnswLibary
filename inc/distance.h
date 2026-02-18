#ifndef VT_DISTANCE_H
#define VT_DISTANCE_H
typedef struct vector vec;

typedef float (*distance_func) (const vec* __restrict a, const vec* __restrict b);

 float l2_sq_distance( const vec* __restrict a,  const vec* __restrict b);
 float cosine_distance(vec* __restrict a, vec* __restrict b);


 float cosine_distance_neon(vec* __restrict a, vec* __restrict b);
 // arm neon smid implementations
 float l2_sq_distance_neon(const vec* __restrict a, const vec* __restrict b);
 float l2_sq_distance_neon_128v(const vec* __restrict a, const vec* __restrict b);
 float l2_sq_distance_neon_128_unroll(const vec *__restrict a, const vec *__restrict b);

#endif