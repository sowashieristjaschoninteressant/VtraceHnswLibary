/* we will write a wrapper so i can still compile with gcc at the end so the bench is a bit mor fair idk trying to have everything the same*/

// benchmark/hnswlib_wrapper.cpp
#include "hnswlib/hnswlib.h"
#include <cstdlib>
#include <cstring>

extern "C" {

typedef struct HnswlibIndex HnswlibIndex;
typedef struct HnswlibBFIndex HnswlibBFIndex;

HnswlibIndex* hnswlib_init(int dim, int max_elements, int M, int ef_construction) {
    hnswlib::L2Space *space = new hnswlib::L2Space(dim);
    hnswlib::HierarchicalNSW<float> *index = 
        new hnswlib::HierarchicalNSW<float>(space, max_elements, M, ef_construction);
    return (HnswlibIndex*)index;
}

void hnswlib_add(HnswlibIndex *idx, float *vec, int id) {
    auto *index = (hnswlib::HierarchicalNSW<float>*)idx;
    index->addPoint(vec, id);
}

void hnswlib_set_ef(HnswlibIndex *idx, int ef) {
    auto *index = (hnswlib::HierarchicalNSW<float>*)idx;
    index->setEf(ef);
}

void hnswlib_search(HnswlibIndex *idx, float *query, int K, int *out_ids) {
    auto *index = (hnswlib::HierarchicalNSW<float>*)idx;
    auto result = index->searchKnn(query, K);
    int i = K - 1;
    while (!result.empty()) {
        out_ids[i]   = result.top().second;
        result.top().first;
        result.pop();
        i--;
    }
}

void hnswlib_free(HnswlibIndex *idx) {
    delete (hnswlib::HierarchicalNSW<float>*)idx;
}

HnswlibBFIndex* hnswlib_bf_init(int dim, int max_elements) {
    auto *space = new hnswlib::L2Space(dim);
    auto *bf = new hnswlib::BruteforceSearch<float>(space, max_elements);
    return (HnswlibBFIndex*)bf;
}

void hnswlib_bf_add(HnswlibBFIndex *idx, float *vec, int id) {
    auto *bf = (hnswlib::BruteforceSearch<float>*)idx;
    bf->addPoint(vec, id);
}

void hnswlib_bf_search(HnswlibBFIndex *idx, float *query, int K, int *ids) {
    auto *bf = (hnswlib::BruteforceSearch<float>*)idx;
    auto result = bf->searchKnn(query, K);

    int i = K - 1;
    while (!result.empty()) {
        ids[i--] = result.top().second;
        result.pop();
    }
}

void hnswlib_bf_free(HnswlibBFIndex* bf){
    delete (hnswlib::BruteforceSearch<float>*)bf;
}

} 