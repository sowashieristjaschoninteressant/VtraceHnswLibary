#ifndef HNSWL_H
#define HNSWL_H

#if defined(__GNUC__) || defined(__clang__)
#define HNSW_API __atribute__((visibility("default")))
#else
#define HNSW_API
#endif
#include"stdlib.h"


typedef struct vector vec;

typedef struct hnsw_result_set hnswResult;
typedef struct Graph HNSW;
 extern HNSW* hnsw_init(uint32_t ef);
 extern void hnsw_insert(HNSW* graph, vec* vector, int32_t M);
 extern int hnsw_search(HNSW* graph, vec* query, int32_t k, hnswResult* results);
 extern void hnsw_free(HNSW* graph);
#endif