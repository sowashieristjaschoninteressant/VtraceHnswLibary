#ifndef HNSW_HEAP_H
#define HNSW_HEAP_H
#include "vtraceCommon.h"

typedef int (*cmp)(void* a, void* b);

extern int min_cmp(void* a, void* b);
extern int max_cmp(void*a, void*b);

HNSW_INLINE uint32 left(uint32 i);
HNSW_INLINE uint32 right(uint32 i);
HNSW_INLINE uint32 parent(uint32 i);

typedef struct {
    void** data;
    uint32 size;
    uint32 capacity;

    cmp compareFunc;
} Heap;

Heap* heap_init(uint32 capacity, cmp cmpFunc);

void heap_insert(Heap* heap, void* data);
#endif