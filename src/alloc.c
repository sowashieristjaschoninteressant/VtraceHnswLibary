#include "alloc.h"

void* hnsw_alloc(size_t size){
    if(size <= 0 ){
        HNSW_LOG("alloc(0) is invalid");
        exit(EXIT_FAILURE);
    }
    
    void* mem = malloc(size);
    if(!mem){
        HNSW_LOG("out of memory");
        exit(EXIT_FAILURE);
    }
    return mem;
}

void hnsw_free(void* ptr){
    free(ptr);
}