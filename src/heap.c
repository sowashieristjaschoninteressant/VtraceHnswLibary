#include "heap.h"

HNSW_INLINE uint32 parent(uint32 i) { return (i - 1) / 2; }
HNSW_INLINE uint32 left(uint32 i)   { return 2*i + 1; }
HNSW_INLINE uint32 right(uint32 i)  { return 2*i + 2; }

Heap* heap_init(uint32 capacity, cmp cmpFunc){

    Heap* heap = malloc(sizeof(Heap));
    
    HNSW_ASSERT(heap);
    heap->data = malloc(sizeof(void*) * capacity);

    HNSW_ASSERT(heap->data);

    heap->capacity = capacity;
    heap->compareFunc = cmpFunc;
    heap->size = 0;

    return heap;
}