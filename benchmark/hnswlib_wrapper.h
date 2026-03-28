#ifndef HNSWLIB_WRAPPER_H
#define HNSWLIB_WRAPPER_H

typedef struct HnswlibIndex HnswlibIndex;
typedef struct HnswlibBFIndex HnswlibBFIndex;

HnswlibIndex* hnswlib_init(int dim, int max_elements, int M, int ef_construction);
void hnswlib_add(HnswlibIndex *idx, float *vec, int id);
void hnswlib_set_ef(HnswlibIndex *idx, int ef);
void hnswlib_search(HnswlibIndex *idx, float *query, int K, int *out_ids);
void hnswlib_free(HnswlibIndex *idx);
HnswlibBFIndex* hnswlib_bf_init(int dim, int max_elements);
void hnswlib_bf_add(HnswlibBFIndex *idx, float *vec, int id);
void hnswlib_bf_search(HnswlibBFIndex *idx, float *query, int K, int *ids);
void hnswlib_bf_free(HnswlibBFIndex* bf);
#endif