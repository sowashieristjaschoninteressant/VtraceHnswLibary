#include "heap.h"

HNSW_INLINE uint32 parent(uint32 i) { 
    return (i - 1) / 2; 
}
HNSW_INLINE uint32 left(uint32 i)   {
     return 2*i + 1;
    }
HNSW_INLINE uint32 right(uint32 i)  {
     return 2*i + 2;
    }

HNSW_INLINE void swap(void** a, void** b){
    void* temp = *a;

    *a = *b;
    *b = temp;
}

HNSW_INLINE void siftDown(Heap* heap, uint32 index){
    while(1){
        uint32 l = left(index), r = right(index);
        uint32 best = index;

        if(l < heap->size && heap->compareFunc(heap->data[l], heap->data[best]) < 0){
            best = l;
        }
        if(r < heap->size && heap->compareFunc(heap->data[r], heap->data[best]) < 0 ){
            best = r;
        }
        if(best == index){
            break;
        }
        swap( (void**) &heap->data[index], (void**) &heap->data[best]);
        index = best;
    }
}

/**
 * if x < 0
 * a has higher prio
 * if x > 0
 * b has higher prio
 * 
 * if x == 0 
 * equal
 * 
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
        // we care later about reallocation basically this should never happen i will allways have fixed size heap
        printf("[-] heap has aleready reached maximum size\n");
        exit(EXIT_FAILURE);
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


heapItem* heapPop(Heap* heap){
    heapItem* root;

    if(heap->size == 0){
        return NULL;
    }

    root = heap->data[0];
    heap->size--;
    heap->data[0] = heap->data[heap->size];

   siftDown(heap, 0);
   
    return root;
}

heapItem* heapPeek(Heap* heap){
   HNSW_ASSERT(heap->size != 0);
    return heap->data[0];
}



