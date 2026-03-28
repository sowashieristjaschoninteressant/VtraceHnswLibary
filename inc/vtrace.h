/**
THIS IS A PROJECT FOR A UNIVERSITY THESIS SO THERE IS ABSOLUTELY NO WARRENTY OF USING THIS system 
*/


#ifndef VTRACE_H
#define VTRACE_H

#include "vtrace_internal.h"


#if defined(__GNUC__) || defined(__clang__)
#define HNSW_API __attribute__((visibility("default")))
#else
#define HNSW_API
#endif



#define DEFAULT_MAX_ELEMENTS 1000000
#define DEFAULT_MAX_LAYERS 10
#define DEFAULT_LEVEL_MAG(Mmax)  1 / log(Mmax);

#define HNSW_OK 0
#define HNSW_ERROR -1
#define HNSW_ENOMEM -2

struct hnsw_result_set{
    size_t size;
    int32* ids;
    float32* distances;
};

typedef struct hnsw_result_set hnswResult;


int populateresultSet(Graph*g, hnswResult* resultSet, int32 size);

// public API
typedef struct Graph HNSW;
HNSW_API extern HNSW* hnsw_init(uint32_t ef);
HNSW_API extern void hnsw_insert(HNSW* graph, vec* vector, int32_t M);
HNSW_API extern int hnsw_search(HNSW* graph, vec* query, int32_t k, hnswResult* results);
HNSW_API extern void hnsw_linear(HNSW* g, vec *q, int K, int *out_ids);
HNSW_API extern void hnsw_free(HNSW* graph);
HNSW_API extern void hnsw_set_ef(HNSW* g, int ef);
#endif