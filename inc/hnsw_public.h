#ifndef HNSWL_H
#define HNSWL_H

#if defined(__GNUC__) || defined(__clang__)
#define HNSW_API __attribute__((visibility("default")))
#else
#define HNSW_API
#endif
#include"stdlib.h"
#include "stdint.h"
#include "vtraceCommon.h"

struct hnsw_result_set{
    size_t size;
    int32_t* ids;
    float* distances;
};

typedef struct hnsw_result_set hnswResult;


typedef struct hnsw_result_set hnswResult;
typedef struct Graph HNSW;
extern HNSW* hnsw_init(uint32_t ef, int32_t dim);
extern void hnsw_insert(HNSW* graph, float32* vector, int32_t M);
extern int hnsw_search(HNSW* graph, float32* query, int32_t k, hnswResult* results);
extern void hnsw_free(HNSW* graph);
extern void hnsw_linear(HNSW* g, float32 *q, int K, int *out_ids);
extern void hnsw_set_ef(HNSW* g, int ef);
#endif
