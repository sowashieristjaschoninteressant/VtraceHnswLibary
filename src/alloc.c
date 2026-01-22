#include "alloc.h"


void* hnsw_alloc_mem(size_t size){
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

void hnsw_free_mem(void* ptr){
    free(ptr);
}


// this will be here the arena stuff


 hnswArena* init_arena(uint32 size){

    hnswArena* arena = malloc(sizeof(hnswArena));

    if(!arena){
         HNSW_LOG("cannot allocate space for arena");
         abort();
    }

    arena->base = malloc(sizeof(uint8) *  size);
   
    if(!arena->base){
        HNSW_LOG("cannot allocate space for arena Base Pointer");
        abort();
    }

    arena->offset = 0;
    arena->size = size;

   

    return arena;

 }


 void arena_destroy(hnswArena* arena){
    free(arena->base);
    free(arena);
 }


 // in order to store every object correctly we need to check for alignment
 // some datatypes like structs or even integers with uin64 bytes need to lay at addresses that are a multible of 8 or 16 for the cpu to be able to accsess it correctly
 void* arena_alloc( hnswArena* arena ,uint32 bytes, uint32 alignment){
    uint32 alignedOffset = align_up(arena->offset, alignment);
    void* ptr;
    
    if(bytes > arena->size  - alignedOffset){
        HNSW_LOG("arena is full!");
        return NULL;
    }

    ptr = arena->base + alignedOffset;
    
    arena->offset = alignedOffset + bytes;

    return ptr;
 }
