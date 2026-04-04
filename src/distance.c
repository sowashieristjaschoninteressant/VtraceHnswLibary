#include "distance.h"
#include "vtrace.h"


// shit when i compile with optimisation flags the floating point comparision is dying so i might have a problem here
float l2_sq_distance(const vec *__restrict a, const vec *__restrict b)
{
    float dist = 0.0f;

    for (uint32 i = 0; i < a->dim; i++)
    {

        float diff = a->vec[i] - b->vec[i];
        
        dist += diff * diff;
    }

    return dist;
}

float cosine_distance(vec *__restrict a, vec *__restrict b)
{
    float dotProd = 0.0f;
    float a_norm, b_norm;
    a_norm = b_norm = 0.0f;

    for (uint32 i = 0; i < a->dim; i++)
    {
        dotProd += a->vec[i] * b->vec[i];

        a_norm += (a->vec[i] * a->vec[i]);
        b_norm += (b->vec[i] * b->vec[i]);
    }

    return dotProd / (sqrtf(a_norm) * sqrtf(b_norm));
}



#ifdef __ARM_NEON
float cosine_distance_neon(vec *__restrict a, vec *__restrict b)
{
    uint32_t i = 0;

    float32x4_t dot_acc = vdupq_n_f32(0.0);
    float32x4_t a_norm_acc = vdupq_n_f32(0.0);
    float32x4_t b_norm_acc = vdupq_n_f32(0.0);

    for (; i + 4 <= a->dim; i += 4)
    {

        float32x4_t va = vld1q_f32(a->vec + i);
        float32x4_t vb = vld1q_f32(b->vec + i);

        dot_acc = vfmaq_f32(dot_acc, va, vb);       // dot product
        a_norm_acc = vfmaq_f32(a_norm_acc, va, va); // a squared
        b_norm_acc = vfmaq_f32(b_norm_acc, vb, vb); // b squared
    }

    float dot = vaddvq_f32(dot_acc);
    float a_norm = vaddvq_f32(a_norm_acc);
    float b_norm = vaddvq_f32(b_norm_acc);

    for (; i < a->dim; i++)
    {
        dot += a->vec[i] * b->vec[i];
        a_norm += a->vec[i] * a->vec[i];
        b_norm += b->vec[i] * b->vec[i];
    }
    
    return dot / (sqrtf(a_norm) * sqrtf(b_norm));
}



/*
when i objdumped the lib i belive i saw aleready some smid instructions but u can never be sure
*/
float32 l2_sq_distance_neon(const vec *__restrict a, const vec *__restrict b)
{

    uint32_t i = 0;
    float32x4_t sum_vec = vdupq_n_f32(0.0f);

    for (; i + 4 <= a->dim; i += 4)
    {
        float32x4_t va = vld1q_f32(a->vec + i);
        float32x4_t vb = vld1q_f32(b->vec + i);

        float32x4_t diff = vsubq_f32(va, vb);

        sum_vec = vfmaq_f32(sum_vec, diff, diff);
    }

    float sum = vaddvq_f32(sum_vec);

    for (; i < a->dim; i++)
    {
        float d = a->vec[i] - b->vec[i];
        sum += d * d;
    }

    return sum;
}

float l2_sq_distance_neon_128v(const vec *__restrict a, const vec *__restrict b)
{
    uint32_t i = 0;

    float32x4_t sum_vec = vdupq_n_f32(0.0f);

    for (; i + 8 <= a->dim; i += 8)
    {
        float32x4_t va1 = vld1q_f32(a->vec + i);
        float32x4_t vb1 = vld1q_f32(b->vec + i);
        float32x4_t va2 = vld1q_f32(a->vec + i + 4);
        float32x4_t vb2 = vld1q_f32(b->vec + i + 4);

        float32x4_t diff1 = vsubq_f32(va1, vb1);
        float32x4_t diff2 = vsubq_f32(va2, vb2);

        sum_vec = vfmaq_f32(sum_vec, diff1, diff1);
        sum_vec = vfmaq_f32(sum_vec, diff2, diff2);
    }

    float sum = vaddvq_f32(sum_vec);

    for (; i < a->dim; i++)
    {
        float d = a->vec[i] - b->vec[i];
        sum += d * d;
    }

    return sum;
}

/**
 sooo i read rolling it out manually helps the compiler i guess i will try that
 */

float l2_sq_distance_neon_128_unroll(const vec *__restrict a, const vec *__restrict b)
{

    float32x4_t sum_vec = vdupq_n_f32(0.0f);

    float32x4_t va0 = vld1q_f32(a->vec + 0);
    float32x4_t vb0 = vld1q_f32(b->vec + 0);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va0, vb0), vsubq_f32(va0, vb0));

    float32x4_t va1 = vld1q_f32(a->vec + 4);
    float32x4_t vb1 = vld1q_f32(b->vec + 4);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va1, vb1), vsubq_f32(va1, vb1));

    float32x4_t va2 = vld1q_f32(a->vec + 8);
    float32x4_t vb2 = vld1q_f32(b->vec + 8);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va2, vb2), vsubq_f32(va2, vb2));

    float32x4_t va3 = vld1q_f32(a->vec + 12);
    float32x4_t vb3 = vld1q_f32(b->vec + 12);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va3, vb3), vsubq_f32(va3, vb3));

    float32x4_t va4 = vld1q_f32(a->vec + 16);
    float32x4_t vb4 = vld1q_f32(b->vec + 16);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va4, vb4), vsubq_f32(va4, vb4));

    float32x4_t va5 = vld1q_f32(a->vec + 20);
    float32x4_t vb5 = vld1q_f32(b->vec + 20);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va5, vb5), vsubq_f32(va5, vb5));

    float32x4_t va6 = vld1q_f32(a->vec + 24);
    float32x4_t vb6 = vld1q_f32(b->vec + 24);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va6, vb6), vsubq_f32(va6, vb6));

    float32x4_t va7 = vld1q_f32(a->vec + 28);
    float32x4_t vb7 = vld1q_f32(b->vec + 28);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va7, vb7), vsubq_f32(va7, vb7));

    float32x4_t va8 = vld1q_f32(a->vec + 32);
    float32x4_t vb8 = vld1q_f32(b->vec + 32);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va8, vb8), vsubq_f32(va8, vb8));

    float32x4_t va9 = vld1q_f32(a->vec + 36);
    float32x4_t vb9 = vld1q_f32(b->vec + 36);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va9, vb9), vsubq_f32(va9, vb9));

    float32x4_t va10 = vld1q_f32(a->vec + 40);
    float32x4_t vb10 = vld1q_f32(b->vec + 40);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va10, vb10), vsubq_f32(va10, vb10));

    float32x4_t va11 = vld1q_f32(a->vec + 44);
    float32x4_t vb11 = vld1q_f32(b->vec + 44);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va11, vb11), vsubq_f32(va11, vb11));

    float32x4_t va12 = vld1q_f32(a->vec + 48);
    float32x4_t vb12 = vld1q_f32(b->vec + 48);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va12, vb12), vsubq_f32(va12, vb12));

    float32x4_t va13 = vld1q_f32(a->vec + 52);
    float32x4_t vb13 = vld1q_f32(b->vec + 52);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va13, vb13), vsubq_f32(va13, vb13));

    float32x4_t va14 = vld1q_f32(a->vec + 56);
    float32x4_t vb14 = vld1q_f32(b->vec + 56);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va14, vb14), vsubq_f32(va14, vb14));

    float32x4_t va15 = vld1q_f32(a->vec + 60);
    float32x4_t vb15 = vld1q_f32(b->vec + 60);
    sum_vec = vfmaq_f32(sum_vec, vsubq_f32(va15, vb15), vsubq_f32(va15, vb15));

    float sum = vaddvq_f32(sum_vec);

    return sum;
}


#elif defined(__AVX2__)
#include "immintrin.h"

     float hsum_avx(__m256 v)
{
    __m128 low  = _mm256_castps256_ps128(v);
    __m128 high = _mm256_extractf128_ps(v, 1);
    __m128 sum4 = _mm_add_ps(low, high);
    __m128 shuf = _mm_movehdup_ps(sum4);
    __m128 sum2 = _mm_add_ps(sum4, shuf);
    __m128 sum1 = _mm_add_ss(sum2, _mm_movehl_ps(shuf, sum2));
    return _mm_cvtss_f32(sum1);
}


// equivalent of l2_sq_distance_neon
float l2_sq_distance_avx2(const vec *__restrict a, const vec *__restrict b)
{
    uint32_t i = 0;
    __m256 sum_vec = _mm256_setzero_ps();

    for (; i + 8 <= a->dim; i += 8)
    {
        __m256 va   = _mm256_loadu_ps(a->vec + i);
        __m256 vb   = _mm256_loadu_ps(b->vec + i);
        __m256 diff = _mm256_sub_ps(va, vb);
        sum_vec     = _mm256_fmadd_ps(diff, diff, sum_vec);
    }

    float32 sum = hsum_avx(sum_vec);

    // scalar tail
    for (; i < a->dim; i++)
    {
        float d = a->vec[i] - b->vec[i];
        sum += d * d;
    }

    return sum;
}

    // equivalent of l2_sq_distance_neon_128v (processes 16 floats per iter)
float l2_sq_distance_avx2_128v(const vec *__restrict a, const vec *__restrict b)
{
    uint32_t i = 0;
    __m256 sum_vec = _mm256_setzero_ps();

    for (; i + 16 <= a->dim; i += 16)
    {
        __m256 va1   = _mm256_loadu_ps(a->vec + i);
        __m256 vb1   = _mm256_loadu_ps(b->vec + i);
        __m256 va2   = _mm256_loadu_ps(a->vec + i + 8);
        __m256 vb2   = _mm256_loadu_ps(b->vec + i + 8);

        __m256 diff1 = _mm256_sub_ps(va1, vb1);
        __m256 diff2 = _mm256_sub_ps(va2, vb2);

        sum_vec = _mm256_fmadd_ps(diff1, diff1, sum_vec);
        sum_vec = _mm256_fmadd_ps(diff2, diff2, sum_vec);
    }

    float sum = hsum_avx(sum_vec);

    for (; i < a->dim; i++)
    {
        float d = a->vec[i] - b->vec[i];
        sum += d * d;
    }

    return sum;
}


// equivalent of l2_sq_distance_neon_128_unroll (fixed 128 dim, fully unrolled)
// AVX2 does 8 floats per register vs NEON's 4, so 16 registers covers 128 floats
float l2_sq_distance_avx2_unroll(const vec *__restrict a, const vec *__restrict b)
{
    __m256 s0 = _mm256_setzero_ps(), s1 = _mm256_setzero_ps();
    __m256 s2 = _mm256_setzero_ps(), s3 = _mm256_setzero_ps();

    #define STEP(s, off) \
        { __m256 d = _mm256_sub_ps(_mm256_loadu_ps(a->vec + off), \
                                    _mm256_loadu_ps(b->vec + off)); \
          s = _mm256_fmadd_ps(d, d, s); }

    STEP(s0,  0) STEP(s1,  8)
    STEP(s2, 16) STEP(s3, 24)
    STEP(s0, 32) STEP(s1, 40)
    STEP(s2, 48) STEP(s3, 56)
    STEP(s0, 64) STEP(s1, 72)
    STEP(s2, 80) STEP(s3, 88)
    STEP(s0, 96) STEP(s1, 104)
    STEP(s2,112) STEP(s3, 120)

    #undef STEP

    __m256 sum_vec = _mm256_add_ps(
        _mm256_add_ps(s0, s1),
        _mm256_add_ps(s2, s3)
    );

    return hsum_avx(sum_vec);
}

// equivalent of cosine_distance_neon
float cosine_distance_avx2(const vec *__restrict a, const vec *__restrict b)
{
    uint32_t i = 0;
    __m256 dot_acc    = _mm256_setzero_ps();
    __m256 a_norm_acc = _mm256_setzero_ps();
    __m256 b_norm_acc = _mm256_setzero_ps();

    for (; i + 8 <= a->dim; i += 8)
    {
        __m256 va = _mm256_loadu_ps(a->vec + i);
        __m256 vb = _mm256_loadu_ps(b->vec + i);

        dot_acc    = _mm256_fmadd_ps(va, vb, dot_acc);
        a_norm_acc = _mm256_fmadd_ps(va, va, a_norm_acc);
        b_norm_acc = _mm256_fmadd_ps(vb, vb, b_norm_acc);
    }

    float dot    = hsum_avx(dot_acc);
    float a_norm = hsum_avx(a_norm_acc);
    float b_norm = hsum_avx(b_norm_acc);

    // scalar tail
    for (; i < a->dim; i++)
    {
        dot    += a->vec[i] * b->vec[i];
        a_norm += a->vec[i] * a->vec[i];
        b_norm += b->vec[i] * b->vec[i];
    }

    return dot / (sqrtf(a_norm) * sqrtf(b_norm));
}

#endif
float l2_sq_distance_fast(const vec *__restrict a,const vec *__restrict b){
    
  #ifdef __ARM_NEON
     
    return l2_sq_distance_neon(a,b);
  #elif defined(__AVX2__)
  
  
    return l2_sq_distance_avx2(a,b);
  
  #else 
   
    return l2_sq_distance(a,b);
  #endif
}
