#include "tests.h"


/*

===========================
HEAP TESTS
===========================

*/

void INSERT_POP_MAXHEAP_TEST(){
    Heap* heap = heap_init(20 , max_cmp);
    float32 testValues[20] = {0.0f, 1.0f,2.0f,3.0f,4.0f,5.0f,6.0f,7.0f,8.0f,9.0f,10.0f,11.0f,12.0f,13.0f,14.0f,15.0f,16.0f,17.0f,18.0f,19.0f};
    for(int32 i = 0; i < 20; i++){
        heap_insert(heap,i, testValues[i]);
    }
    
    heapItem* root = heapPop(heap);
    
    assert(root->dist == testValues[19]);
    printf("[+] INSERT_POP_MAXHEAP_WORKS!\n");
    fflush(stdout);

    heap_dispose(heap);
    return;
}

void HEAP_TESTS(){
   INSERT_POP_MAXHEAP_TEST();

    printf("[+] HEAP TESTS SUCCESSFULLY FINISHED\n");
    return;
}