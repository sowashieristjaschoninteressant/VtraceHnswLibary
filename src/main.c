#include "vtrace.h"
#include "tests.h"
int main(int argc, char** argw){
    HNSW_UNUSED(argc);
    HNSW_UNUSED(argw);
    
    HEAP_TESTS();
    GRAPH_TESTS();
    ARENA_TESTS();
    return EXIT_SUCCESS;
}