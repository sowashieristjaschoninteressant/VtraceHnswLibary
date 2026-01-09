#include "tests.h"



static inline float32 randf(){
    return (float)rand() / (float) RAND_MAX;
}

static inline float32 getRandFloat(float32 min, float32 max){
    return min + (max - min) * randf();
}

// generate randome float array
float32* generateRFA(uint32 size, float32 min, float32 max){

    float32* farray = malloc(sizeof(float32) * size);
    assert(farray);

    for(uint32 i = 0; i < size; i++){
        farray[i] = getRandFloat(min,max);
    }

    return farray;
}


/*

===========================
HEAP TESTS
===========================

*/

void INSERT_POP_MINHEAP_TEST(){
    int32 fsize = 20000;
    Heap* heap = heap_init(fsize, min_cmp);
    float32* farray = generateRFA(fsize, 0.0f, 20000);

    for(int32 i = 0; i < fsize; i++){
        heap_insert(heap, i, farray[i]);
    }

    heapItem* before =  heapPop(heap);
    assert(before != NULL);
    for(int32 i = 1; i < fsize; i++){
       
        heapItem* temp = heapPop(heap);
        assert(temp != NULL); 
        assert( temp->dist >= before->dist);

        before = temp;
    }

    printf("[+] INSERT_POP_MINHEAP WORKS!\n");
    fflush(stdout);
    heap_dispose(heap);
    free(farray);

    return;
}

void INSERT_POP_MAXHEAP_TEST(){
    int32 fsize = 100;
    Heap* heap = heap_init(fsize , max_cmp);
    float32* farray = generateRFA(fsize,0.0f, 20000);

    for(int32 i = 0; i < fsize; i++){

        heap_insert(heap,i, farray[i]);
    }

    heapItem* before = heapPop(heap);
    assert(before != NULL);
    for(int32 i = 1; i < fsize; i++){
       
        heapItem* temp = heapPop(heap);
        assert(temp != NULL);
        
        assert( temp->dist <= before->dist );

        before = temp;
    }
    
    printf("[+] INSERT_POP_MAXHEAP_WORKS!\n");
    fflush(stdout);

    heap_dispose(heap);
    free(farray);
    return;
}

void HEAP_TESTS(){
   INSERT_POP_MAXHEAP_TEST();
    INSERT_POP_MINHEAP_TEST();
    printf("[+] HEAP TESTS SUCCESSFULLY FINISHED\n");
    return;
}