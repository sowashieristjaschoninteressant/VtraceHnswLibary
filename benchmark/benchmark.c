

#include "stdlib.h"
#include "time.h"
#include "stdio.h"
#include "hnsw_public.h"
#include "vtraceCommon.h"
#include "vec.h"


#define VTRACE_TESTS

struct timer{
    struct timespec start;
    struct timespec end;
    double elapsed;
};

void timer_start(struct timer* time){
    clock_gettime(CLOCK_MONOTONIC,&time->start);
}

void timer_stop(struct timer* time){
    clock_gettime(CLOCK_MONOTONIC, &time->end);
    time->elapsed = (time->end.tv_sec - time->start.tv_sec) + (time->end.tv_nsec - time->start.tv_nsec)/1e9;
}

void showTime(struct timer* time){
    printf("Time taken: %.6f seconds\n",time->elapsed);
}

double getElapsed(struct timer* t){

    return t->elapsed;
}

static inline float32 randf()
{
    return (float)rand() / (float)RAND_MAX;
}

static inline float32 getRandFloat(float32 min, float32 max)
{
    return min + (max - min) * randf();
}

// generate randome float array
float32 *generateRandVecS(uint32 size, float32 min, float32 max)
{

    float32 *farray = malloc(sizeof(float32) * size);
    assert(farray);

    for (uint32 i = 0; i < size; i++)
    {
        farray[i] = getRandFloat(min, max);
    }

    return farray;
}

vec createVecR(int dim){
   vec v; 
   v.dim = dim;
   v.vec = generateRandVecS(dim, 0 , 100000);

    return v;
}


vec* genVecArray(int count, int dim){
    vec* vecs = malloc(sizeof(vec) * count);
    assert(vecs);

    for(int i = 0; i < count; i++){
        vecs[i] = createVecR(dim);
    }

    return vecs;
}

FILE* cFile(char* path){

     FILE* fp = fopen(path, "w+");
     assert(fp);

    return fp;
}
/* this method benchmarks the insertion algo in terms of total time for n insertions lets start simple*/
void naive_benchmark_insertion(){
    //HNSW* graph = hnsw_init()
    FILE* filep = cFile("benchmark_insertion.csv");
    fprintf(filep, "N,dimension,M,ef_construction,build_time_ms\n");
    int N;
    const int dim = 128;
    const int M = 16;
    const int ef_construction = 200;
    double build_time_ms;
    
    for(int i = 1000; i < 51000; i += 1000){

        HNSW* graph = hnsw_init(ef_construction);
        struct timer t;
        vec* vecs =  genVecArray(50000,dim);

        timer_start(&t);
        for(int j = 0; j < i; j++){
            hnsw_insert(graph,&vecs[j], M);
        }
        timer_stop(&t);
        
        N = i;
        build_time_ms = getElapsed(&t);
        printf("nodes: %i, elapsed in ms: %f\n", i, build_time_ms);
        fprintf(filep, "%i,%i,%i,%i,%f\n",N,dim,M,ef_construction,build_time_ms);

        for(int c = 0; c < 50000;  c++){
            free(vecs[c].vec);
        }
        free(vecs);
        
        hnsw_free(graph);
    }

    fclose(filep);

}

void benchmark_search_throughput() {
    FILE* filep = fopen("benchmark_search_throughput.csv", "w");
    if(!filep){
        perror("Cannot open CSV file");
        return;
    }
    fprintf(filep, "N,dimension,M,ef_search,avg_latency_ms,qps\n");

    // --- Benchmark parameters ---
    const int dim = 128;         // vector dimension
    const int M = 16;            // max neighbors
    const int ef_search = 200;   // ef search parameter
    const int nodes = 50000;     // number of vectors in the graph
    const int num_queries = 10000; // number of queries to benchmark

    // --- Initialize HNSW graph ---
    HNSW* graph = hnsw_init(ef_search);
    vec* vecs = genVecArray(nodes, dim);
    HNSW_LOG("Inserting vectors into HNSW graph...");
    for (int i = 0; i < nodes; i++) {
        hnsw_insert(graph, &vecs[i], M);
    }

    // --- Generate query vectors ---
    vec* queries = genVecArray(num_queries, dim);
    struct hnsw_result_set r;

    HNSW_LOG("Starting search throughput benchmark...");

    // --- Volatile sink to prevent compiler optimization ---
    volatile float sink = 0.0f;

    struct timer t;
    timer_start(&t);

    for (int i = 0; i < num_queries; i++) {
        hnsw_search(graph, &queries[i], M, &r);

        // accumulate distances to prevent optimization
        for (int j = 0; j < r.size; j++) {
            sink += r.distances[j];
        }
    }

    timer_stop(&t);

    double total_sec = getElapsed(&t);
    double avg_latency_ms = (total_sec / num_queries) * 1000.0; // convert to ms
    double qps = num_queries / total_sec;

    printf("[!] Sink: %.6f (to prevent optimization)\n", sink);
    printf("Search benchmark results:\n");
    printf("Nodes: %d, Queries: %d, Avg latency: %.3f ms, Throughput: %.2f QPS\n",
           nodes, num_queries, avg_latency_ms, qps);

    fprintf(filep, "%d,%d,%d,%d,%.6f,%.2f\n",
            nodes, dim, M, ef_search, avg_latency_ms, qps);

    // --- Clean up ---
    for (int i = 0; i < nodes; i++) free(vecs[i].vec);
    free(vecs);

    for (int i = 0; i < num_queries; i++) free(queries[i].vec);
    free(queries);

    fclose(filep);

    // Free HNSW graph
    hnsw_free(graph);
}


void naive_benchmark_search(){
    FILE* filep = cFile("naive_benchmark_search.csv");
    fprintf(filep, "N,dimension,M,ef_search,search_time_ms\n");
    
    int N;
    const int dim = 128;
    const int M = 16;
    const int ef_search = 200;
    double search_time_ms;
    // fill the graph
    const int nodes = 50000;
    HNSW* graph = hnsw_init(ef_search);
    vec* vecs = genVecArray(nodes,dim);
    HNSW_LOG("inserting vectores for now...");
    for(int i = 0; i < nodes; i++){

        hnsw_insert(graph,&vecs[i],M);
    }
    struct hnsw_result_set r;
    HNSW_LOG("STARTING SEARCH TESTS!");
    for(int i = 1000; i < 51000; i += 1000){

        struct timer t;
        
        vec* qvecs = genVecArray(i, dim);

        timer_start(&t);
        for(int j = 0; j < i; j++){
          
            hnsw_search(graph,&qvecs[j], M, &r);
        }
        timer_stop(&t);
        
        N = i;
        search_time_ms = getElapsed(&t);
        
        

        printf("nodes: %i, elapsed in ms: %f\n", i, search_time_ms);
        fprintf(filep, "%i,%i,%i,%i,%f\n",N,dim,M, ef_search,search_time_ms);
    for(int c = 0; c < i; c++){
            free(qvecs[c].vec);
        }
        free(qvecs);

    
    }

    
    fclose(filep);    


}

int main(){

    
    benchmark_search_throughput();
    

}