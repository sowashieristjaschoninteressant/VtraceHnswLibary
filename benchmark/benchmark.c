

#include "hnsw_public.h"


#define VTRACE_TESTS

struct Dataset
{
    float *data;
    int n;
    int dim;
};

typedef struct Dataset Dataset;

Dataset load_fvecs(const char *path)
{
    Dataset ds;

    FILE *f = fopen(path, "rb");

    if (!f)
    {
        printf("dataset cannot be opend?\n");
        abort();
    }
    printf("okaz is it loadin\n");
    fflush(stdout);
    int dim;
    fread(&dim, 4, 1, f);

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    int vec_size = 4 + dim * 4;

    ds.n = sz / vec_size;
    ds.dim = dim;

    ds.data = malloc(ds.n * dim * sizeof(float));
    assert(ds.data);
    for (int i = 0; i < ds.n; i++)
    {   
        fread(&dim, 4, 1, f);
        fread(ds.data + i * dim, 4, dim, f);
    }
    printf("dataset %s loaded!\n", path);
    fclose(f);
    return ds;
}

void destroy_dataset(Dataset ds)
{
    free(ds.data);
}

struct timer
{
    struct timespec start;
    struct timespec end;
    double elapsed;
};

void timer_start(struct timer *time)
{
    clock_gettime(CLOCK_MONOTONIC, &time->start);
}

void timer_stop(struct timer *time)
{
    clock_gettime(CLOCK_MONOTONIC, &time->end);

    double sec = time->end.tv_sec - time->start.tv_sec;
    double nanos = time->end.tv_nsec - time->start.tv_nsec;

    time->elapsed = sec * 1000 + nanos / 1e6;
}

void showTime(struct timer *time)
{
    printf("Time taken: %.6f seconds\n", time->elapsed);
}

 double getElapsed(struct timer *t)
{

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

vec createVecR(int dim)
{
    vec v;
    v.dim = dim;
    v.vec = generateRandVecS(dim, 0, 100000);

    return v;
}

vec *genVecArray(int count, int dim)
{
    vec *vecs = malloc(sizeof(vec) * count);
    assert(vecs);

    for (int i = 0; i < count; i++)
    {
        vecs[i] = createVecR(dim);
    }

    return vecs;
}

FILE *create_file(char *path)
{

    FILE *fp = fopen(path, "w+");
    assert(fp);

    return fp;
}

/*1. load index 2. setup timer 3. */
void benchmark_insertion(int id)
{
    // HNSW* graph = hnsw_init()
    char filename[256]; // make sure it's big enough
    snprintf(filename, sizeof(filename), "benchmark_insertion_%d.csv", id);

    FILE *filep = create_file(filename);
    Dataset ds = load_fvecs("./benchmark/datasets/sift/sift_base.fvecs");
    struct timer t;
    const int ef = 100;
    const int M = 12;
    fprintf(filep, "dimension,M,ef_construction,build_time_ms,avg_insert_us,vec_count\n");

    for (int ef = 500; ef <= 500; ef += 50)
    {

        HNSW *hnsw = hnsw_init(ef);

        vec tmp;
        tmp.dim = ds.dim;

        timer_start(&t);
        for (int i = 0; i < ds.n; i++)
        {
            float *v = ds.data + i * ds.dim;
            tmp.vec = v;
            hnsw_insert(hnsw, &tmp, M);
        }
        timer_stop(&t);

        double avg_us = t.elapsed * 1000 / ds.n;

        printf("avg insert: %.3f µs\n", avg_us);

        fprintf(filep, "%i,%i,%i,%.3f,%.3f,%i\n", ds.dim, M, ef, getElapsed(&t), avg_us, ds.n);
        fflush(filep);
        hnsw_free(hnsw);
    }

    // write to csv mabey even do it with different ef values to get a cool diagram for my thesis

    fclose(filep);
    destroy_dataset(ds);
}

void benchmark_search(int id)
{   
    char filename[256]; // make sure it's big enough
    snprintf(filename, sizeof(filename), "benchmark_search_%d.csv", id);
    FILE *filep = create_file(filename);

    fprintf(filep, "N,dimension,M,ef_search,avg_latency_ms,qps, recall, numQueries\n");
    printf("okay loading vecs!\n");
    // first load the base vecs
    Dataset baseV = load_fvecs("./benchmark/datasets/sift/sift_base.fvecs");
    const int ef = 200;
    const int M = 10;
    HNSW *hnsw = hnsw_init(ef);
    vec tmp;

    tmp.dim = baseV.dim;
    for (int i = 0; i < baseV.n; i++)
    {   
        float *v = baseV.data + i * baseV.dim;
        tmp.vec = v;
       
        hnsw_insert(hnsw, &tmp, M);
    }

    destroy_dataset(baseV);
    // load queries
    Dataset qv = load_fvecs("./benchmark/datasets/sift/sift_query.fvecs");
    
    tmp.dim = qv.dim;
    struct timer t;

    double totalLatency = 0.0;
    int correct = 0;
    int numQueries = qv.n;
    const int K = 10;
    double recall;
    
    HNSW_LOG("Okay allocating Stuff ");
    hnswResult* tempRset = malloc(sizeof(hnswResult) * numQueries);
    assert(tempRset);

    // warmup 
    for(int i = 0; i < 1000; i++){
    tmp.vec = qv.data + (i % numQueries) * qv.dim;
    hnsw_search(hnsw, &tmp, K, &tempRset[0]);
}
    
    timer_start(&t);
    for(int i = 0; i < numQueries; i++){
     tmp.vec = qv.data + i * qv.dim;
     hnsw_search(hnsw,&tmp,K, &tempRset[i]);

    }
    timer_stop(&t);

    // get recall
    int* bestIds = malloc(sizeof(int) * K);
    for(int i = 0; i < numQueries; i++){
        tmp.vec = qv.data + i * qv.dim;
        hnsw_linear(hnsw,&tmp,K, bestIds);
        // check distances 

       
        for(int j = 0; j < K; j++){
            
            for(int k = 0; k < K; k++){
           if(tempRset[i].ids[k] == bestIds[j]){
            correct++;
            break;
         }
             }
        }
        }
        

     recall = (double) correct / (numQueries * K);

   double QPS = numQueries / (t.elapsed / 1000.0);
    // write CSV
    fprintf(filep, "%d,%d,%d,%d,%.6f,%.2f,%.3f,%d\n",
            numQueries, tmp.dim, M, ef,
            t.elapsed / numQueries, // avg latency ms
            QPS,      // QPS
            recall,
            numQueries);
    free(bestIds);
    free(tempRset);
    destroy_dataset(qv);
    hnsw_free(hnsw);
    fclose(filep);
}

int main()
{

#ifdef __AVX2__
    printf("AVX2 is defined in benchmark.c\n");
#else
    printf("AVX2 NOT defined in benchmark.c\n");
#endif

     for(int i = 0; i < 1;i++){
        printf("benchmark insertion round: %i\n", i);
        benchmark_insertion(i);
    }


    for(int i = 0; i < 5;i++){
        printf("benchmark round: %i\n", i);
        benchmark_search(i);
    }

}
