#include "distance.h"

#include "vtrace.h"
// shit when i compile with optimisation flags the floating point comparision is dying so i might have a problem here
float32 l2_sq_distance(const vec *__restrict a, const vec *__restrict b)
{
    float32 dist = 0.0f;

    for (uint32 i = 0; i < a->dim; i++)
    {

        float32 diff = a->vec[i] - b->vec[i];

        dist += diff * diff;
    }

    return dist;
}

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

float cosine_distance(vec *__restrict a, vec *__restrict b)
{
    float dotProd = 0.0f;
    float a_norm, b_norm;
    a_norm = b_norm = 0.0f;

    for (int i = 0; i < a->dim; i++)
    {
        dotProd += a->vec[i] * b->vec[i];

        a_norm += (a->vec[i] * a->vec[i]);
        b_norm += (b->vec[i] * b->vec[i]);
    }

    return dotProd / (sqrtf(a_norm) * sqrtf(b_norm));
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

    float32 sum = vaddvq_f32(sum_vec);

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

    float32 sum = vaddvq_f32(sum_vec);

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

    // First 64 floats
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
