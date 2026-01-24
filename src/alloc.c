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



 hnsw_chainAllocator* init_chainArena(uint32 size, uint32 chunkSize){
    hnsw_chainAllocator* chainArena = malloc(sizeof(chainArena));

    if(!chainArena){
        HNSW_LOG("cannot allocate chainAllocator out of memory");
        abort();
    }

    chainArena->arenaPtr = malloc(sizeof(hnswArena*) * size);

    if(!chainArena->arenaPtr){
        HNSW_LOG("cannot alloate arenaPtrArray out of memory?");
        abort();
    }

    for(uint32 i = 0; i < size; i++){

        chainArena->arenaPtr[i] = init_arena(chunkSize);
    }

    chainArena->arraySize = size;
    chainArena->chunkSize = chunkSize;

    return chainArena;

 }
 void chainArena_destroy(hnsw_chainAllocator* chainAllocator){
    if(!chainAllocator || !chainAllocator->arenaPtr) return;

    for(uint32 i = 0; i < chainAllocator->arraySize; i++){
        arena_destroy(chainAllocator->arenaPtr[i]);
    }

    free(chainAllocator->arenaPtr);
 }
 void* chainArenaAlloc( hnsw_chainAllocator* chainAllocator ,uint32 size, uint32 alignment){
    
    void* ptr = arena_alloc( chainAllocator->arenaPtr[chainAllocator->current], size, alignment);
    
    if(ptr){
        return ptr;
    }

    if(chainAllocator->current + 1 < chainAllocator->arraySize){
        chainAllocator->current++;
        return arena_alloc(
            chainAllocator->arenaPtr[chainAllocator->current],
            size,
            alignment
        );
    }

    /* if not we need to grow chainArena*/

    uint32 oldSize = chainAllocator->arraySize;
    uint32 newSize = oldSize * 2;

    chainAllocator->arraySize = newSize;
    void* new_ptr = realloc(chainAllocator->arenaPtr, chainAllocator->arraySize * sizeof(*chainAllocator->arenaPtr));
    
    if(!new_ptr){
        HNSW_LOG("cannot reallocate arenaChunks out of memory?");
        abort();
    }

    chainAllocator->arenaPtr = new_ptr;

    for(uint32 i = oldSize; i < newSize; i++){
        chainAllocator->arenaPtr[i] = init_arena(chainAllocator->chunkSize);
    }
    chainAllocator->current = oldSize;

    return arena_alloc(chainAllocator->arenaPtr[chainAllocator->current], size, alignment);
 }
