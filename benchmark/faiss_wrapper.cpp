#include <faiss/IndexHNSW.h>
#include <faiss/IndexFlat.h>
#include <faiss/Index.h>
#include <cstdint>

extern "C" {
    #include "faiss_wrapper.h"

    FaissIndex* faiss_init(int dim, int M, int ef_construction){
        faiss::IndexHNSWFlat* index = new faiss::IndexHNSWFlat(dim, M, faiss::METRIC_L2);
        index->hnsw.efConstruction = ef_construction;
        return (FaissIndex*)index;
    }

    void faiss_add(FaissIndex* idx, float* data, int n){
        auto *index = (faiss::IndexHNSWFlat*)idx;
        index->add(n,data);
    }

    void faiss_set_ef(FaissIndex* idx, int ef){
        auto *index = (faiss::IndexHNSWFlat*)idx;
        index->hnsw.efSearch = ef;
    }

    void faiss_search(FaissIndex* idx, float* query, int K, int* out_ids){
        auto *index = (faiss::IndexHNSWFlat*) idx;
        
        std::vector<faiss::Index::idx_t> labels(K);
        std::vector<float> dists(K);

        index->search(1,query,K,dists.data(),labels.data());

        for(int i = 0; i < K; i++){
            out_ids[i] = (int) labels[i];
        }
    }

    void faiss_free(FaissIndex* idx){
        delete (faiss::IndexHNSWFlat*)idx;
    }

    FaissIndexFlat* faiss_bf_init(int dim){
        faiss::IndexFlatL2* index = new faiss::IndexFlatL2(dim);
        return (FaissIndexFlat*)index;
    }

    void faiss_bf_add(FaissIndexFlat* idx, int size, float* vecs){
        auto* index = (faiss::IndexFlatL2*)idx;
        index->add(size,vecs);
    }

    void faiss_linear(FaissIndexFlat* idx, float* query, int K, int* ids){
        auto* index = (faiss::IndexFlatL2*)idx;

        std::vector<faiss::Index::idx_t> labels(K);
        std::vector<float> dists(K);

        index->search(1,query, K,dists.data(),labels.data());

        for(int i = 0; i < K; i++){
            ids[i] = (int)labels[i];
        }
    }

    void faiss_bf_free(FaissIndexFlat* idx){
        delete (faiss::IndexFlatL2*)idx;
    }

}