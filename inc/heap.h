#ifndef HNSW_HEAP_H
#define HNSW_HEAP_H
#include "vtraceCommon.h"


struct heapItem {

    void* data;
    uint32 id;
    float32 dist; // key
};
typedef struct heapItem heapItem;

typedef int32 (*cmp)(const heapItem* a, const heapItem* b);
 struct h_heap {
    heapItem* data;
    
    uint32 size;
    uint32 capacity;

    cmp compareFunc;
};

typedef struct h_heap Heap;

// UTILS
extern int32 min_cmp( const heapItem* a, const heapItem* b);
extern int32 max_cmp( const heapItem* a, const heapItem* b);

HNSW_INLINE uint32 left(uint32 i);
HNSW_INLINE uint32 right(uint32 i);
HNSW_INLINE uint32 parent(uint32 i);
HNSW_INLINE void swap(heapItem* a, heapItem* b);
HNSW_INLINE void siftDown(Heap* heap, uint32 index);

// exposed api functions
extern Heap* heap_init(uint32 capacity, cmp cmpFunc);
extern void heap_dispose(Heap* heap);
extern void heap_insert(Heap* heap, uint32 id, float32 dist, void* data);
extern heapItem heapPop(Heap* heap);
extern heapItem heapPeek(Heap* heap);
extern void heap_reset(Heap* heap);
extern void debugPrintHeap(Heap* heap);

#define MAX_HEAP(capacity) heap_init((uint32) capacity, max_cmp);
#define MIN_HEAP(capacity) heap_init((uint32) capacity, min_cmp);

#endif