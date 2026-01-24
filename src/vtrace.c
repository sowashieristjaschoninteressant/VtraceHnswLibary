#include "vtrace.h"

HNSW* hnsw_init(int max_elements, int M,  uint32 efConstruction, float32 level_multiplyer);
void hnsw_insert(HNSW* graph, int id, float* vector);
void hnsw_search(HNSW* graph, float* query, int k, int efSearch, int* results);
void hnsw_free(HNSW* graph);