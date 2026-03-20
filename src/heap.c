#include "heap.h"
#include "alloc.h"

HNSW_INLINE uint32 parent(uint32 i) { 
    return (i - 1) / 2; 
}
HNSW_INLINE uint32 left(uint32 i)   {
     return 2*i + 1;
    }
HNSW_INLINE uint32 right(uint32 i)  {
     return 2*i + 2;
    }

HNSW_INLINE void swap(heapItem* a, heapItem* b){
    heapItem temp = *a;

     *a = *b;
     *b = temp;
}

HNSW_INLINE void siftDown(Heap* heap, uint32 index){
    while(1){
        uint32 l = left(index), r = right(index);
        uint32 best = index;

        if(l < heap->size && heap->compareFunc(&heap->data[l], &heap->data[best]) < 0){
            best = l;
        }
        if(r < heap->size && heap->compareFunc(&heap->data[r], &heap->data[best]) < 0 ){
            best = r;
        }
        if(best == index){
            break;
        }
        swap(  &heap->data[index],  &heap->data[best]);
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

    Heap* heap = hnsw_alloc_mem(sizeof(Heap), alignof(Heap));
    
    HNSW_ASSERT(heap);
    heap->data = malloc(sizeof(heapItem) * capacity);
    
    HNSW_ASSERT(heap->data);
   

    heap->capacity = capacity;
    heap->compareFunc = cmpFunc;
    heap->size = 0;

    return heap;
}

void minToMaxHeap(Heap* heap){
    heap->compareFunc = &max_cmp;
    heapify(heap);
    
}

void maxToMinHeap(Heap* heap){
    heap->compareFunc = &min_cmp;
    heapify(heap);
}

void heap_dispose(Heap* heap){
    hnsw_free_mem(heap->data);
    hnsw_free_mem(heap);
}



void heap_insert(Heap* heap, uint32 id, float32 dist, void* data){
   if(heap->size >= heap->capacity){
        // increase capacity
        int32 oldCap = heap->capacity;        
        heap->capacity = oldCap * 2;

        heap->data = realloc(heap->data, sizeof(heapItem) *  heap->capacity);
        HNSW_ASSERT(heap->data);
    }

   uint32 i = heap->size++;
   heap->data[i].dist = dist;
   heap->data[i].id = id;
   heap->data[i].data = data;

    // Percolate up
    while(i > 0){
        uint32 p = parent(i);
        if(heap->compareFunc(&heap->data[p], &heap->data[i]) <= 0) break;
        swap( &heap->data[i], &heap->data[p]);
        i = p;
    }

}

heapItem heapPop(Heap* heap){
    heapItem root;
    HNSW_ASSERT(heap->size > 0);

    root = heap->data[0];
    heap->size--;
    heap->data[0] = heap->data[heap->size];

   siftDown(heap, 0);
   
    return root;
}

inline heapItem heapPeek(Heap* heap){
   HNSW_ASSERT(heap->size != 0);
    return heap->data[0];
}

void heap_reset(Heap* heap){

    heap->size = 0;
}

void debugPrintHeap(Heap* heap){

    for(uint32 i = 0; i < heap->size; i++){

        printf("id=%i, dist=%.2f\n", heap->data[i].id, heap->data[i].dist);
    }
}

void heapify(Heap* heap) {
    for (int32 i = heap->size / 2 - 1; i >= 0; i--) {
        siftDown(heap, i);
    }
}

sortedBuffer* initSortedBuffer(size_t size){
    sortedBuffer* buffer = malloc(sizeof(sortedBuffer));
    assert(buffer);
    buffer->capacity = size;
    buffer->data = malloc(sizeof(heapItem) *  buffer->capacity);
    assert(buffer->data);
    buffer->size = 0;

    return buffer;

}

void destroySortedBuffer(sortedBuffer* buf){
    free(buf->data);
    free(buf);
}

void push_buffer(sortedBuffer* buf, heapItem* data){

    if(buf->size >= buf->capacity){
       int32 newCap = buf->capacity ? buf->capacity * 2 : 4;
       if (newCap < buf->capacity) abort(); // overflow guard
       
       heapItem* newPtr = realloc(buf->data, sizeof(heapItem) * newCap);
       if(!newPtr){
        HNSW_LOG("okay sorted buffer cannot realloc fuu");
        abort();
        }
        buf->capacity = newCap;
        buf->data = newPtr;
    }

    buf->data[buf->size++] = *data;
     
}

DirtyBuffer* initDirtyBuffer(size_t size){
    DirtyBuffer* buf = malloc(sizeof(DirtyBuffer));
    assert(buf);

    buf->capacity = size;
    buf->data = malloc(sizeof(DirtyItem) *  size);
    assert(buf->data);

    return buf;
}

void push_dirtyBuffer(DirtyBuffer* buf, DirtyItem data){

    if(buf->size >= buf->capacity){
       int32 newCap = buf->capacity ? buf->capacity * 2 : 4;
       if (newCap < buf->capacity) abort(); // overflow guard
       
       DirtyItem* newPtr = realloc(buf->data, sizeof(DirtyItem) * newCap);
       if(!newPtr){
        HNSW_LOG("okay sorted buffer cannot realloc fuu");
        abort();
        }
        buf->capacity = newCap;
        buf->data = newPtr;
    }

    buf->data[buf->size++] = data;

}



void heap_drain_to_sorted_buffer(Heap* heap, sortedBuffer* buffer){
 
    int32 originalSize = heap->size;
    
    while (heap->size > 0){
        heapItem item = heapPop(heap);
        push_buffer(buffer, &item);
    }
 
   
}