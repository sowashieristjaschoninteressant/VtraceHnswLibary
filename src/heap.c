#include "heap.h"

HNSW_INLINE uint32 parent(uint32 i) { return (i - 1) / 2; }
HNSW_INLINE uint32 left(uint32 i)   { return 2*i + 1;}
HNSW_INLINE uint32 right(uint32 i)  { return 2*i + 2;}

HNSW_INLINE void swap(void** a, void** b){
    void* temp = *a;

    *a = *b;
    *b = temp;
}

/**
 * @brief okay this function is a bit complicated for me so ill write the meaning of the possible return values (market as x for now) here
 * if x < 0
 * a has higher prio
 * if x > 0
 * b has higher prio
 * 
 * if x == 0 
 * equal
 * 
 * @param a 
 * @param b 
 * @return HNSW_INLINE 
 */
 int32 min_cmp(const heapItem* a, const heapItem* b){
    return (a->dist > b->dist) - (a->dist < b->dist);
}

 int32 max_cmp( const heapItem* a, const heapItem* b){
    return (a->dist < b->dist) - (a->dist > b->dist);
}

Heap* heap_init(uint32 capacity, cmp cmpFunc){

    Heap* heap = malloc(sizeof(Heap));
    
    HNSW_ASSERT(heap);
    heap->data = malloc(sizeof(heapItem*) * capacity);
    heap->pool = malloc(sizeof(heapItem) * capacity);
    HNSW_ASSERT(heap->data);
    HNSW_ASSERT(heap->pool);

    heap->capacity = capacity;
    heap->compareFunc = cmpFunc;
    heap->size = 0;

    return heap;
}

void heap_dispose(Heap* heap){
    free(heap->pool);
    free(heap->data);
    free(heap);
}

void heap_insert(Heap* heap, uint32 id, float32 dist){
    int32 i;

    if(heap->size > heap->capacity){
        printf("[-] ok here we need to realloc...\n");
        heap->capacity = heap->capacity * 2;
        realloc(heap->data, heap->capacity);
    }

    heapItem* item = (heapItem*) &heap->pool[heap->size];

    item->id = id;
    item->dist = dist;
    
    heap->data[heap->size] = item;
    i = heap->size++;

    while(i > 0){
        uint32 p = parent(i);

        if(heap->compareFunc(heap->data[p], heap->data[i]) <= 0){
            break;
        }

        swap( (void**) &heap->data[i],(void**) &heap->data[p]);
        i = p;
    }

}

/**
 * @brief returns the first item out of the heap.
 * for now we just return the heapItem and dont exclude it
 * @param heap 
 * @return heapItem* 
 */
heapItem* heapPop(Heap* heap){
    int32 root = 0;
    if(heap->size == 0){
        return NULL;
    }
    // pop and close out logic;

    return heap->data[root];
}



