#include "tests.h"
#include "stdio.h"
#include "string.h"


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

    HNSW_LOG("[+] INSERT_POP_MINHEAP WORKS!");
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
    
    HNSW_LOG("INSERT_POP_MAXHEAP_WORKS!\n");
    fflush(stdout);

    heap_dispose(heap);
    free(farray);
    return;
}

void HEAP_TESTS(){
   INSERT_POP_MAXHEAP_TEST();
    INSERT_POP_MINHEAP_TEST();
    HNSW_LOG("HEAP TESTS SUCCESSFULLY FINISHED");
    return;
}
vec make_vec(uint32 dim, float32* values) {

    vec v;
    v.vec = malloc(sizeof(float32) * dim);
    HNSW_ASSERT(v.vec);
    v.dim = dim;

    
    memcpy(v.vec, values, sizeof(float32) * dim);

    return v;

}

void SIMPLE_SEARCH_LAYERTEST(){

    // prepare
    Graph* graph = initializeGraph(0,4,4, 2);
    
    float32 vals0[] = {0.0f,0.0f};
    float32 vals1[] = {1.0f,0.0f};
    float32 vals2[] = {0.0f,1.0f};
    float32 vals3[] = {1.0f,1.0f};
    
    const uint32 vectorCount = 4;
    const uint32 M_MAX = 2;
    vec testVectores[vectorCount];

    testVectores[0] = make_vec(2, vals0);
    testVectores[1] = make_vec(2, vals1);
    testVectores[2] = make_vec(2, vals2);
    testVectores[3] = make_vec(2, vals3);
    

    
    node nodes[4];
    for(uint32 i = 0; i < 4; i++){
        
        nodes[i].data = &testVectores[i];
         

        nodes[i].numNeigbours = malloc(sizeof(uint32) * 2);
        HNSW_ASSERT(nodes[i].numNeigbours);
        nodes[i].numNeigbours[0] = 0;

       // allocate neighbour lists
        nodes[i].neigbours = malloc(sizeof(uint32*) * 2);
        HNSW_ASSERT(nodes[i].neigbours);
        nodes[i].neigbours[0] = malloc(sizeof(uint32) * M_MAX);
         HNSW_ASSERT(nodes[i].neigbours[0]);
         nodes[i].id = i;
    }

   

    // ARRANGE
    VTaddNeigbour(&nodes[0], 1, 0, M_MAX);
    VTaddNeigbour(&nodes[0], 2, 0, M_MAX);

    VTaddNeigbour(&nodes[1], 0, 0, M_MAX);
    VTaddNeigbour(&nodes[1], 3, 0, M_MAX);

    VTaddNeigbour(&nodes[2], 0, 0, M_MAX);
    VTaddNeigbour(&nodes[2], 3, 0, M_MAX);

    VTaddNeigbour(&nodes[3], 1, 0, M_MAX);
    VTaddNeigbour(&nodes[3], 2, 0, M_MAX);


    graph->nodes = &nodes;
    graph->count = 4;
    graph->entrypoint = &nodes;

   
    vec query = make_vec(2, (float[]) {0.1f, 0.1f});
     printf("okay running SEARCH-LAYER!\n");
        fflush(stdout);
    Heap* results = SEARCH_LAYER(graph, query, 0);

    while(results->size > 0){

      heapItem* temp =  heapPop(results);
      printf("these are the results id:  %i distance: %f\n", temp->id, temp->dist);
    }

    heap_dispose(results);
    
}


 

 void SEARCHLAYER_TESTS(){
    HNSW_LOG("STARTING SEARCHLAYER TESTS");
    SIMPLE_SEARCH_LAYERTEST();
}