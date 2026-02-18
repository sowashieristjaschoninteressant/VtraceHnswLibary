#include "tests.h"
#include "stdio.h"
#include "string.h"

static inline float32 randf()
{
    return (float)rand() / (float)RAND_MAX;
}

static inline float32 getRandFloat(float32 min, float32 max)
{
    return min + (max - min) * randf();
}

// generate randome float array
float32 *generateRandVec(uint32 size, float32 min, float32 max)
{

    float32 *farray = malloc(sizeof(float32) * size);
    assert(farray);

    for (uint32 i = 0; i < size; i++)
    {
        farray[i] = getRandFloat(min, max);
    }

    return farray;
}

Graph *make_simpleTestGraph(uint32 maxNodeCount)
{
    const uint32 maxLayer = 10;
    const uint32 maxNeigbors = 5;
    const uint32 ef = 10;
    Graph *graph = initializeGraph(maxLayer, ef, ef, maxNeigbors, maxNodeCount);
    HNSW_ASSERT(graph);
    // lets reseed here for a more deterministic graph
    srand(42);

    return graph;
}

void validate_graph(Graph *g)
{
    for (uint32 i = 0; i < g->count; i++)
    {
        node *n = &g->nodes[i];
        for (uint32 l = 0; l <= n->level; l++)
        {
            HNSW_ASSERT(n->numNeigbours[l] <= g->M_maxNeigbours);
            for (uint32 j = 0; j < n->numNeigbours[l]; j++)
            {
                uint32 nb = n->neigbours[(l * g->M_maxNeigbours) + j];
                HNSW_ASSERT(nb < g->count);
                HNSW_ASSERT(nb != i);
            }
        }
    }
}

// this graph will be just a simple one layer graph with fixed values in terms of M_MAXNEIGBOURS and MAXLAYER we just will have 4 nodes that will be connected with a max of 2 connections
Graph *mockGraphOneLayer(uint32 efSearch)
{
    const uint32 M_MAX_LAYER = 0;
    const uint32 EF_CONSTRUCTION = 40; // irrelevant
    const uint32 M_MAXNEIGBOURS = 2;
    const uint32 vectorCount = 4;
    const uint32 LAYERS = 1;
    const uint32 maxNodeCount = 50000;

    Graph *graph = initializeGraph(M_MAX_LAYER, EF_CONSTRUCTION, efSearch, M_MAXNEIGBOURS, maxNodeCount);

    float32 vals0[] = {0.0f, 0.0f};
    float32 vals1[] = {1.0f, 0.0f};
    float32 vals2[] = {0.0f, 1.0f};
    float32 vals3[] = {1.0f, 1.0f};

    vec *testVectores = hnsw_alloc_mem(sizeof(vec) * vectorCount, alignof(vec));
    HNSW_ASSERT(testVectores);

    testVectores[0] = make_vec(2, vals0);
    testVectores[1] = make_vec(2, vals1);
    testVectores[2] = make_vec(2, vals2);
    testVectores[3] = make_vec(2, vals3);

    node *nodes = hnsw_alloc_mem(sizeof(node) * 4, alignof(node));
    HNSW_ASSERT(nodes);

    for (uint32 i = 0; i < 4; i++)
    {

        nodes[i].v = testVectores[i];

        nodes[i].numNeigbours = hnsw_alloc_mem((sizeof(uint32) * LAYERS), alignof(uint32));
        HNSW_ASSERT(nodes[i].numNeigbours);
        nodes[i].numNeigbours[0] = 0;

        nodes[i].neigbours = hnsw_alloc_mem( sizeof(uint32) * (M_MAXNEIGBOURS * LAYERS), alignof(uint32));
        
        nodes[i].id = i;
    }

    addNeigbour(&nodes[0], 1, 0, M_MAXNEIGBOURS);
    addNeigbour(&nodes[0], 2, 0, M_MAXNEIGBOURS);

    addNeigbour(&nodes[1], 0, 0, M_MAXNEIGBOURS);
    addNeigbour(&nodes[1], 3, 0, M_MAXNEIGBOURS);

    addNeigbour(&nodes[2], 0, 0, M_MAXNEIGBOURS);
    addNeigbour(&nodes[2], 3, 0, M_MAXNEIGBOURS);

    addNeigbour(&nodes[3], 1, 0, M_MAXNEIGBOURS);
    addNeigbour(&nodes[3], 2, 0, M_MAXNEIGBOURS);
    

    graph->nodes = nodes;
    graph->count = 4;
    graph->entrypointID = nodes[0].id;

    return graph;
}

// 5 nodes node w id  = 5 is connected with 3 and 1 
Graph* mockGraphTwoLayer(int32 efSearch){

    const uint32 M_MAX_LAYER = 1;
    const uint32 EF_CONSTRUCTION = 40; // irrelevant
    const uint32 M_MAXNEIGBOURS = 3;
    const uint32 vectorCount = 5;
    const uint32 LAYERS = 2;
    const uint32 maxNodeCount = 50000;

    Graph *graph = initializeGraph(M_MAX_LAYER, EF_CONSTRUCTION, efSearch, M_MAXNEIGBOURS, maxNodeCount);

    float32 vals0[] = {0.0f, 0.0f};
    float32 vals1[] = {1.0f, 0.0f};
    float32 vals2[] = {0.0f, 1.0f};
    float32 vals3[] = {1.0f, 1.0f};
    float32 vals4[] = {0.95, 0.95f};

    vec *testVectores = hnsw_alloc_mem(sizeof(vec) * vectorCount, alignof(vec));
    HNSW_ASSERT(testVectores);

    testVectores[0] = make_vec(2, vals0);
    testVectores[1] = make_vec(2, vals1);
    testVectores[2] = make_vec(2, vals2);
    testVectores[3] = make_vec(2, vals3);
    testVectores[4] = make_vec(2, vals4);

    node *nodes = hnsw_alloc_mem(sizeof(node) * 5, alignof(node));
    HNSW_ASSERT(nodes);

    for (uint32 i = 0; i < vectorCount; i++)
    {

        nodes[i].v = testVectores[i];

        nodes[i].numNeigbours = hnsw_alloc_mem((sizeof(uint32) * LAYERS), alignof(uint32));
        HNSW_ASSERT(nodes[i].numNeigbours);
        nodes[i].numNeigbours[0] = 0;

        nodes[i].neigbours = hnsw_alloc_mem(sizeof(uint32) * (M_MAXNEIGBOURS * LAYERS), alignof(uint32));
        HNSW_ASSERT(nodes[i].neigbours);

       
        nodes[i].id = i;
    }

    addNeigbour(&nodes[0], 1, 0, M_MAXNEIGBOURS);
    addNeigbour(&nodes[0], 2, 0, M_MAXNEIGBOURS);

    addNeigbour(&nodes[1], 0, 0, M_MAXNEIGBOURS);
    addNeigbour(&nodes[1], 3, 0, M_MAXNEIGBOURS);

    addNeigbour(&nodes[2], 0, 0, M_MAXNEIGBOURS);
    addNeigbour(&nodes[2], 3, 0, M_MAXNEIGBOURS);

    addNeigbour(&nodes[3], 1, 0, M_MAXNEIGBOURS);
    addNeigbour(&nodes[3], 2, 0, M_MAXNEIGBOURS);

    addNeigbour(&nodes[4], 3, 0, M_MAXNEIGBOURS);
    addNeigbour(&nodes[4], 1, 0, M_MAXNEIGBOURS);

    addNeigbour(&nodes[3], 4, 0, M_MAXNEIGBOURS);
    addNeigbour(&nodes[1], 4, 0, M_MAXNEIGBOURS);

    graph->nodes = nodes;
    graph->count = 5;
    graph->entrypointID = nodes[0].id;

    return graph;
}

/*

===========================
HEAP TESTS
===========================

*/

void INSERT_POP_MINHEAP_TEST()
{
    int32 fsize = 20000;
    Heap *heap = heap_init(fsize, min_cmp);
    float32 *farray = generateRandVec(fsize, 0.0f, 20000);

    for (int32 i = 0; i < fsize; i++)
    {
        heap_insert(heap, i, farray[i], NULL);
    }

    heapItem before = heapPop(heap);

    for (int32 i = 1; i < fsize; i++)
    {

        heapItem temp = heapPop(heap);

        HNSW_ASSERT(before.dist <= temp.dist);
        before = temp;
    }

    HNSW_LOG("[+] INSERT_POP_MINHEAP WORKS!");
    fflush(stdout);
    heap_dispose(heap);
    hnsw_free_mem(farray);

    return;
}

void INSERT_POP_MAXHEAP_TEST()
{
    int32 fsize = 100;
    Heap *heap = heap_init(fsize, max_cmp);
    float32 *farray = generateRandVec(fsize, __FLT_MIN__, __FLT_MAX__);

    for (int32 i = 0; i < fsize; i++)
    {

        heap_insert(heap, i, farray[i], NULL);
    }

    heapItem before = heapPop(heap);

    for (int32 i = 1; i < fsize; i++)
    {

        heapItem temp = heapPop(heap);

        assert(temp.dist <= before.dist);

        before = temp;
    }

    HNSW_LOG("INSERT_POP_MAXHEAP_WORKS!");
    fflush(stdout);

    heap_dispose(heap);
    free(farray);
    return;
}

void HEAP_TESTS()
{
    HNSW_LOG("STARTING HEAP TESTS");
    INSERT_POP_MAXHEAP_TEST();
    INSERT_POP_MINHEAP_TEST();
    DETERMINISTIC_INSERTPOP_TEST();
    HNSW_LOG("HEAP TESTS SUCCESSFULLY FINISHED");
    return;
}

void DETERMINISTIC_INSERTPOP_TEST()
{
    const uint32 size = 10;
    Heap *heap = heap_init(size, max_cmp);
    float32 sValues[size] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for (uint32 i = 0; i < size; i++)
    {
        heap_insert(heap, i, sValues[i], NULL);
    }

    uint32 i = 9;
    while (heap->size > 0)
    {
        heapItem current = heapPop(heap);

        HNSW_ASSERT(current.dist == sValues[i--]);
    }

    HNSW_LOG("DETERMINISTIC INSERTPOP WORKS!");
}

/*

===========================
Arena Tests
===========================

*/

void ARENA_TESTS()
{
    HNSW_LOG("STARTING ARENA TESTS");
    ARENA_ALLOCATEBIG_TEST();
    ARENA_ALLOCATEOVERFLOW_TEST();
    ARENA_ALIGNMENT_TEST();
    CHAINARENA_READWRITEALLOCATION_TEST();
    CHAINARENA_USENEXT_TEST();
    CHAINARENA_GROW_TEST();
    HNSW_LOG("ENDING ALLOCATION TESTS");
}


void ARENA_ALLOCATEBIG_TEST()
{
    uint32 arenaSize = KB(1);
    int8 magicNr = 0x5;
    hnswArena *arena = init_arena(arenaSize);

    HNSW_ASSERT(arena);
    HNSW_ASSERT(arena->base);
    HNSW_ASSERT(arena->size == arenaSize);

    int8 *byteArray = arena_alloc(arena, arenaSize, alignof(int8));
    HNSW_ASSERT(byteArray);

    memset(byteArray, magicNr, arenaSize);

    for (uint32 i = 0; i < arenaSize; i++)
    {
        HNSW_ASSERT(byteArray[i] == magicNr);
    }

    HNSW_LOG("ARENA ALLOCATION of 1KB works");
    arena_destroy(arena);
}

void ARENA_ALLOCATEOVERFLOW_TEST()
{

    hnswArena *arena = init_arena(5); // 5 bytes arena

    HNSW_ASSERT(arena->base);
    HNSW_ASSERT(arena);

    int8 *byteArray = arena_alloc(arena, 10, 1);

    HNSW_ASSERT(!byteArray);

    HNSW_LOG("ALLOCATOR CATCHES OVERFLOW!");
    HNSW_LOG("Arena allocation overflow test succsessfull!");
    arena_destroy(arena);
}

void ARENA_ALIGNMENT_TEST()
{

    hnswArena *arena = init_arena(KB(1));

    HNSW_ASSERT(arena);
    HNSW_ASSERT(arena->base);

    hnswNode *node = arena_alloc(arena, sizeof(hnswNode), alignof(hnswNode));
    HNSW_ASSERT(node);

    HNSW_ASSERT(((uintptr_t)node % alignof(hnswNode)) == 0);
    // try to accsess data in the pointer check for missalignment errors
    node->id = 0;
    node->level = 10;
    node->numNeigbours = arena_alloc(arena, sizeof(uint32) * 10, alignof(uint32));

    HNSW_ASSERT(((uintptr_t)node % alignof(uint32)) == 0);

    HNSW_ASSERT(node->numNeigbours);
    Graph *graph = arena_alloc(arena, sizeof(Graph), alignof(Graph));
    HNSW_ASSERT(graph);
    // try to accsess data in the pointer check for missalignment errors
    graph->efsearch = 20;
    graph->maxLayer = 30;

    HNSW_LOG("ARENA ALIGNMENT TESTS SUCCSESSFULLY FINISHED!");

    arena_destroy(arena);
}

void CHAINARENA_READWRITEALLOCATION_TEST(){

    hnsw_chainAllocator* carena = init_chainArena(1, KB(1) / 2 );

    HNSW_ASSERT(carena);

    int32 arraySize = 30;


    float32* testArray = chainArenaAlloc(carena, sizeof(float32) * arraySize, alignof(float32));

    

    for(int i = 0; i < arraySize; i++){
        testArray[i] = 5.325f;
        HNSW_ASSERT(testArray[i] == 5.325f);
    }

    HNSW_LOG("chainallocationTest readWrite to data success!");

    chainArena_destroy(carena);
}


void CHAINARENA_USENEXT_TEST(){
    uint32 arenas = 3;
    int32 arenaSize = 10;
    
    hnsw_chainAllocator* carena = init_chainArena(arenas, arenaSize );
    uint32 oldOffset = carena->current;

    int8* testBytes = chainArenaAlloc(carena, 10,alignof(int8));
    int8* newByteAllocation = chainArenaAlloc(carena, 10,alignof(int8));

    HNSW_ASSERT(oldOffset != carena->current);
    HNSW_ASSERT(testBytes);
    HNSW_ASSERT(newByteAllocation);

    HNSW_LOG("use next arena in array test SUCCESS!");

    chainArena_destroy(carena);
    
}

void CHAINARENA_GROW_TEST(){
    // prepare
    const int32 arenas = 3;
    const uint32 chunkSize = 20;
    const uint32 magicNr = 0x5;
    int8* byteptr;
    hnsw_chainAllocator* carena = init_chainArena(arenas, chunkSize);
  
    // arange

    for(int i = 0; i < 3; i++){
        chainArenaAlloc(carena, chunkSize, sizeof(int8));
    }

    byteptr = chainArenaAlloc(carena,10, sizeof(int8));
    memset(byteptr, magicNr, 10);
    
    // assert
    HNSW_ASSERT(byteptr);
    HNSW_ASSERT(carena->arraySize == arenas * 2);

    for(int i = 0; i < 10; i++){
       HNSW_ASSERT(byteptr[i] == magicNr);
    }

    HNSW_LOG("CHAINARENA grow works!");
}



/*
===========================
Graph Tests
===========================
*/

void SELECT_NEAREST_NABOURS()
{

    // Example distances for candidates
    float distances[] = {10.5, 2.3, 7.7, 4.4, 6.6, 1.1, 8.8};
    uint32 num_candidates = sizeof(distances) / sizeof(distances[0]);
    uint32 M = 3; 
    Graph* g = mockGraphOneLayer(20);
    //Grapg* g = initializeGraph()
    Heap *c = heap_init(num_candidates, max_cmp);
    for (uint32 i = 0; i < num_candidates; i++)
    {
        heap_insert(c, i, distances[i], NULL);
    }

    printf("Candidates in max-heap order (root = largest distance):\n");
    debugPrintHeap(c);

    // Step 2: Select M nearest neighbors
    Heap *m = SELECT_NEIGBOURS_SIMPLE(g,c, M);

    // Step 3: Pop from the resulting max-heap to get M closest
    printf("\nSelected %u nearest neighbors (max-heap root = farthest of the closest):\n", M);
    while (m->size > 0)
    {
        heapItem item = heapPop(m);
        printf("id=%u dist=%.2f\n", item.id, item.dist);
    }

    // Cleanup
    heap_dispose(c);
}

void SIMPLE_SEARCH_LAYERTEST()
{

    // prepare
    Graph *graph = mockGraphOneLayer(4);
    vec query = make_vec(2, (float[]){0.1f, 0.1f});
    hnswNode* ep = getNodeById(graph, graph->entrypointID);
    sortedBuffer*results = SEARCH_LAYER(graph,ep, query, graph->efsearch, 0);

    const uint32 expectedResultIds[] = {0, 2, 1, 3};
    int resultIteration = 0;
    for(int32 i = 0; i < results->size; i++)
    {

        heapItem temp = results->data[i];
        HNSW_ASSERT(temp.id == expectedResultIds[resultIteration++]);
        //printf("these are the results id:  %i distance: %f\n", temp.id, temp.dist);
    }
    HNSW_LOG("SEARCH-LAYER WORKING");
    
}

void INIT_GRAPH_TEST()
{

    const uint32 maxLayer = 10;
    const uint32 efSearch = 20;
    const uint32 efConstruction = 40;
    const uint32 M_maxNeigbours = 10;
    const uint32 maxNodeCount = 50000;
    Graph *graph = initializeGraph(maxLayer, efConstruction, efSearch, M_maxNeigbours, maxNodeCount);

    HNSW_ASSERT(graph);
    HNSW_ASSERT(graph->efconstruction == efConstruction);
    HNSW_ASSERT(graph->efsearch == efSearch);
    HNSW_ASSERT(graph->M_maxNeigbours == M_maxNeigbours);
    HNSW_ASSERT(graph->maxLayer == maxLayer);
    HNSW_ASSERT(graph->nodes);
    HNSW_ASSERT(graph->storage.candidateHeap);
    HNSW_ASSERT(graph->storage.simpleHeap);
    HNSW_ASSERT(graph->storage.discardedHeap);
    HNSW_ASSERT(graph->storage.closestResults);
    HNSW_ASSERT(graph->storage.resultHeap);
    HNSW_ASSERT(graph->visited.visited);

    uninitializeGraph(graph);
    HNSW_LOG("GRAPH INITIALISATION WORKS SUCESSFULLY");
}

void GRAPH_TESTS()
{
    HNSW_LOG("STARTING GRAPH TESTS");
    INIT_GRAPH_TEST();
    SIMPLE_SEARCH_LAYERTEST();

    SELECT_NEAREST_NABOURS();
    INSERT_TESTS();
    ANN_SEARCH_TEST_ONE_LAYER();
    ANN_SEARCH_TEST_HEAP_TRIMMING();
    ANN_SEARCH_TEST_CHANGE_ENTRYPOINT();
    ANN_SEARCH_TEST_TWO_LAYERS_MOCK();
    FULL_API_INSERT_SEARCH_TEST();
    HNSW_LOG("GRAPH TESTS RUN SUCESSFULLY!");
}

void FULL_API_INSERT_SEARCH_TEST()
{   
    const int32 maxSize = 5000;
    const int32 dim = 5;
    Graph* g = initializeGraph(10, 20, 10, 20, maxSize);
    vec* vecs = malloc(sizeof(vec) * maxSize);
    assert(vecs);

    float32 ml = 1 / log(g->M_maxNeigbours);
    
    for(int32 i = 0; i < maxSize; i++){
        float* fvalues = generateRandVec(dim,0,100000); 
        vecs[i] = make_vec(dim,fvalues);

        INSERT(g, vecs[i],10 ,g->M_maxNeigbours, g->efconstruction, ml);
    }

    vec query = make_vec(dim, (float[]) {0.1,5.0, 6,2,3});

    Heap* r = K_NN_SEARCH(g, query, 3, g->efsearch);

    debugPrintHeap(r);
    validate_graph(g);
    HNSW_LOG("OKAY FULL API WORKS!");

}

void ANN_SEARCH_TEST_TWO_LAYERS_MOCK(){
    HNSW_LOG("START 2 LAYER TEST!");
    const int32 expectedID = 4;
    Graph* g = mockGraphTwoLayer(10);
    vec q = make_vec(2, (float[]){ 0.95f, 0.95f});
    
    Heap* K = K_NN_SEARCH(g,q,1,10);

    HNSW_ASSERT(expectedID == heapPeek(K).id);
    HNSW_ASSERT(K->size == 1);
    // NOTE: due to manually inserting the nodes its possible that the graph structure is not preserved i will try this now with the official api
    //validate_graph(g);
    
    
}

void ANN_SEARCH_TEST_CHANGE_ENTRYPOINT(){
    HNSW_LOG("start ANN_SEARCH TEST CHANGING ENTRYPOINT");
    const int32 ef = 10;
    const int32 expectedID = 3;
    Graph* g = mockGraphOneLayer(ef);
    
    g->entrypointID = 3; // huuh this is a bit crazy

    vec q = make_vec(2, (float[]) {0.9f,0.9f});
    
    Heap* w = K_NN_SEARCH(g,q,1,ef);

    HNSW_ASSERT(w);
    HNSW_ASSERT(w->size ==  1 );
    HNSW_ASSERT(heapPeek(w).id == expectedID);
    HNSW_LOG("despite entrypoint change graph returns correct result!!");
    uninitializeGraph(g);

}

void ANN_SEARCH_TEST_HEAP_TRIMMING(){

    HNSW_LOG("start ANN_SEARCH HEAP_TRIMM TEST!");
    const int32 ef = 4, expectedID = 3, K = 2;
    Graph* g = mockGraphOneLayer(ef);
    
    vec q = make_vec(2, (float[]) {0.9f,0.9f});
    
    Heap* w = K_NN_SEARCH(g,q,K,ef);
    printf("this is the current heapSize: %i\n", w->size); fflush(stdout);
    debugPrintHeap(w); fflush(stdout);
    printf("end\n");    
    HNSW_ASSERT(w);
    HNSW_ASSERT(w->size == K );
    HNSW_ASSERT(heapPeek(w).id == expectedID || w->data[1].id == expectedID );
    HNSW_LOG("OKAY ANN_SEARCH RETURNS CORRECT HEAP SIZE");
    uninitializeGraph(g);

}


void ANN_SEARCH_TEST_ONE_LAYER(){
    HNSW_LOG("start ANN_SEARCH TEST!");
    const int32 ef = 10;
    const int32 expectedID = 3;
    Graph* g = mockGraphOneLayer(ef);
    
    vec q = make_vec(2, (float[]) {0.9f,0.9f});
    
    Heap* w = K_NN_SEARCH(g,q,1,ef);

    HNSW_ASSERT(w);
    HNSW_ASSERT(w->size ==  1 );
    HNSW_ASSERT(heapPeek(w).id == expectedID);
    HNSW_LOG("OKAY ANN_SEARCH RETURNS CORRECT RESULT ONE LAYER GRAPH MOCK!");
    uninitializeGraph(g);
}

void INSERT_TESTS()
{
    test_insert_first_node();
    test_bidirectionalLinks();
    test_max_neigbours_respected();
    test_expandGraph();
}
void test_insert_first_node()
{
    // arrange
    const uint32 maxNodeCount = 200; 
    Graph *graph = make_simpleTestGraph(maxNodeCount);
    float32 fValues[] = {3, 4, 5};
    vec v = make_vec(3, fValues);
    uint32 M = 10;

    float32 ml = 1 / log(graph->M_maxNeigbours);

    INSERT(graph, v, M, graph->M_maxNeigbours, 200, ml);

    // assert
    HNSW_ASSERT(graph->count == 1);
    HNSW_ASSERT(graph->entrypointID == graph->nodes[0].id);
    HNSW_ASSERT(graph->maxLayer == graph->nodes[0].level);

    validate_graph(graph);

    HNSW_LOG("insert first node works!");
    uninitializeGraph(graph);
}

void test_bidirectionalLinks()
{
    Graph *graph = make_simpleTestGraph(50);
    uint32 dim = 4;
    vec vecs[50];

    // seed the random algorithm to smth more deterministic

    srand(42);
    // arange
    float32 ml = 1 / log(graph->M_maxNeigbours);

    for (uint32 j = 0; j < 50; j++)
    {
        vecs[j].vec = generateRandVec(dim, 0, 100000);
        vecs[j].dim = dim;

        INSERT(graph, vecs[j], 10, 200, 100, ml);
    }

    // assert
    // this is shit but i cannot found out on another way
    for (uint32 i = 0; i < graph->count; i++)
    {
        hnswNode *node = &graph->nodes[i];

        for (uint32 l = 0; l <= node->level; l++)
        {
            for (uint32 k = 0; k < node->numNeigbours[l]; k++)
            {
                uint32 nid = node->neigbours[l * graph->M_maxNeigbours + k];

                hnswNode *other = &graph->nodes[nid];

                bool found = false;
                for (uint32 kk = 0; kk < other->numNeigbours[l]; kk++)
                {
                    if (other->neigbours[l * graph->M_maxNeigbours + kk] == i)
                    {
                        found = true;
                    }
                }

                HNSW_ASSERT(found);
                {
                }
            }
        }
    }

    uninitializeGraph(graph);
    HNSW_LOG("GRAPH CONNECTIONS ARE BIDIRECTIONAL!");
}

void test_max_neigbours_respected()
{
    const uint32 vecsize = 10000;

    Graph *g = make_simpleTestGraph(vecsize);
    uint32 dim = 150;
    
    vec *vecs = malloc(sizeof(vec) * vecsize);
    HNSW_ASSERT(vecs);

    // arange
    float32 ml = 1 / log(g->M_maxNeigbours);

    for (uint32 j = 0; j < vecsize; j++)
    {
        vecs[j].vec = generateRandVec(dim, 0, 1000000);
        vecs[j].dim = dim;
        INSERT(g, vecs[j], 10, 10, 50, ml);
    }

    for (uint32 i = 0; i < g->count; i++)
    {
        hnswNode *n = &g->nodes[i];

        for (int lc = 0; lc < n->level; lc++)
        {
            HNSW_ASSERT(n->numNeigbours[lc] <= g->M_maxNeigbours);
        }
    }

    validate_graph(g);

    for (int i = 0; i < vecsize; i++)
    {
        free(vecs[i].vec);
    }
    free(vecs);
    uninitializeGraph(g);
    HNSW_LOG("max neigbours respected succsess!! no realloc in this test");
}

void test_expandGraph()
{
    const int32 vCount = 500;
    const uint32 dim = 5;
   
    // declare
    HNSW_LOG("STARTING expandGraph TEST");
    // less nodes as max firstly
    Graph *g = make_simpleTestGraph(vCount / 2 );

    const uint32 firstMaxNodes = g->maxNodeCount;
    const float32 ml = 2 / log(g->M_maxNeigbours);

    vec v[vCount];

    // arrange
    for (int i = 0; i < vCount; i++)
    {
        v[i].vec = generateRandVec(dim, 0, 1000000);
        v[i].dim = dim;

        //printf("insertion round: %lu\n", i);
        INSERT(g, v[i], 10, 10, 10, ml);
    }

    HNSW_ASSERT(g->maxNodeCount == firstMaxNodes * 2);
    validate_graph(g);

    HNSW_LOG("EXPANDING GRAPH OVER SET MAX NODES POSSIBLE!");

    uninitializeGraph(g);
}

