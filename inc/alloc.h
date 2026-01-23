#ifndef HNSW_ALLOC_H
#define HNSW_ALLOC_H

#include "vtraceCommon.h"

// i will write the arena firstly here

#define KB(x) (x << 10)
#define MB(x) (x << 20)
#define GB(x) (x << 30)

struct arena{
    uint8* base;
    uint32 offset;
    uint32 size;
};

typedef struct arena hnswArena;

struct chainArena{
    uint8** arenaPtr;
    int32 current;
    uint32 chunkSize;
    uint32 arraySize;
    
};

typedef struct chainArena hnsw_chainAllocator;

extern hnswArena* init_arena(uint32 size);
extern void arena_destroy(hnswArena* arena);
extern void* arena_alloc( hnswArena* arena ,uint32 bytes, uint32 alignment);
extern void arena_reset(hnswArena* arena);
 
extern hnsw_chainAllocator* init_chainArena(uint32 size);
extern void chainArena_destroy(hnsw_chainAllocator* chainAllocator); 
extern void* chainArenaAlloc( hnsw_chainAllocator* chainAllocator ,uint32 size);

static inline uint32 align_up(uint32 value, uint32 alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}


extern void* hnsw_alloc_mem(size_t size);
extern void hnsw_free_mem(void* ptr);




#endif