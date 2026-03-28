#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include "./faiss_wrapper.h"

typedef float float32;
typedef unsigned int uint32;

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

static inline float randf()
{
    return (float)rand() / (float)RAND_MAX;
}

static inline float32 getRandFloat(float32 min, float32 max)
{
    return min + (max - min) * randf();
}


void replace_str(char* str, char old_char, char new_char){
    if(!str) return;

    while(*str != '\0'){

        if(*str == old_char) *str = new_char;
        str++;
    }


}



FILE *create_file(char *path)
{

    FILE *fp = fopen(path, "w+");
    assert(fp);

    return fp;
}

/*1. load index 2. setup timer 3. */
void benchmark_insertion_faisslib(int id, const char* lib_path)
{
    char base_path[256];

    snprintf(base_path, sizeof(base_path), "%s_base.fvecs", lib_path);

    char filename[500]; // make sure it's big enough
    snprintf(filename, sizeof(filename), "faiss_benchmark_insertion_%d_%s.csv", id, lib_path);

    replace_str(filename, '/', '_');

    FILE *filep = create_file(filename);
    Dataset ds = load_fvecs(base_path);
    struct timer t;
    
    const int M = 12;
   
    fprintf(filep, "dimension,M,ef_construction,build_time_ms,avg_insert_us,vec_count\n");

    for (int ef = 50; ef <= 500; ef += 50)
    {
        FaissIndex* index = faiss_init(ds.dim,M,ef);
        
        timer_start(&t);
        for (int i = 0; i < ds.n; i++)
        {   
           
            float *v = ds.data + i * ds.dim;
        
            faiss_add(index,v,1);

        }
        timer_stop(&t);

        double avg_us = t.elapsed * 1000 / ds.n;

        printf("avg insert: %.3f µs\n", avg_us);

        fprintf(filep, "%i,%i,%i,%.3f,%.3f,%i\n", ds.dim, M, ef, getElapsed(&t), avg_us, ds.n);
        fflush(filep);
        faiss_free(index);
    }

    // write to csv mabey even do it with different ef values to get a cool diagram for my thesis

    fclose(filep);
    destroy_dataset(ds);
}


typedef struct {
    int ids[10]; // bc K = 10 for now
} Result;

void benchmark_searchfaisslib(int id, const char *dataset_path)
{

    char base_path[256];
    char query_path[256];

    snprintf(base_path, sizeof(base_path), "%s_base.fvecs", dataset_path);
    snprintf(query_path, sizeof(query_path), "%s_query.fvecs", dataset_path);

    char filename[500]; // make sure it's big enough
    snprintf(filename, sizeof(filename), "faiss_benchmark_search_%d_%s.csv", id, dataset_path);
    replace_str(filename, '/', '_');
    
    FILE *filep = create_file(filename);

    fprintf(filep, "N,dimension,M,ef_search,avg_latency_ms,qps, recall, numQueries\n");
    printf("okay loading vecs!\n");
    // first load the base vecs
    Dataset baseV = load_fvecs(base_path);
    const int ef = 200;
    const int M = 10;
    const int K = 10;
    
    FaissIndex* index = faiss_init(baseV.dim, M,ef);
    
    for (int i = 0; i < baseV.n; i++)
    {
        float *v = baseV.data + i * baseV.dim;
        faiss_add(index, v, 1);
    }

        // load queries
    Dataset qv = load_fvecs(query_path);
    // precompute groundtrhuth

    int *groundtruth = malloc(sizeof(int) * K * qv.n);

    FaissIndexFlat* bf = faiss_bf_init(baseV.dim);

    // build brute-force index
        faiss_bf_add(bf, baseV.n,baseV.data);
    

    // compute GT
    for (int i = 0; i < qv.n; i++) {
    float *q = qv.data + i * qv.dim;
    faiss_linear(bf, q, K, groundtruth + i * K);
    }

 
    struct timer t;

    double totalLatency = 0.0;
    int correct = 0;
    int numQueries = qv.n;

    double recall;

    
    int ef_values[] = {10, 20, 40, 60, 80, 100, 150, 200, 300, 400, 500};
    int num_ef = 11;
    Result* tempRset = malloc(sizeof(Result) * qv.n);

    for (int e = 0; e < num_ef; e++)
    {
        int ef_search = ef_values[e];
        
        faiss_set_ef(index, ef_search);
        // warmup
        for (int i = 0; i < 1000; i++)
        {
            float* vec = qv.data + (i % qv.n) * qv.dim;
            faiss_search(index, vec, K, tempRset[0].ids);
        }

        // timed search
        timer_start(&t);
        for (int i = 0; i < qv.n; i++)
        {
            float* vec = qv.data + i * qv.dim;
            faiss_search(index, vec, K, tempRset[i].ids);
        }
        timer_stop(&t);

        // recall
        int correct = 0;
        for (int i = 0; i < qv.n; i++){
            for (int j = 0; j < K; j++){
                for (int k = 0; k < K; k++){
                    if (tempRset[i].ids[k] == groundtruth[i * K + j])
                    {
                        correct++;
                        break;
                    }
                }
            }
        }
            
        recall = (double)correct / (numQueries * K);

        double QPS = numQueries / (t.elapsed / 1000.0);
        // write CSV
        fprintf(filep, "%d,%d,%d,%d,%.6f,%.2f,%.3f,%d\n",
                numQueries, baseV.dim, M, ef_search,
                t.elapsed / numQueries, // avg latency ms
                QPS,                    // QPS
                recall,
                numQueries);
            fflush(filep);
        }

    

        free(tempRset);
        free(groundtruth);
        destroy_dataset(baseV);
        destroy_dataset(qv);
        faiss_free(index);
        faiss_bf_free(bf);
        fclose(filep);
}

int main()
{

#ifdef __AVX2__
    printf("AVX2 is defined in benchmark.c\n");
#else
    printf("AVX2 NOT defined in benchmark.c\n");
#endif
   
    
    benchmark_insertion_faisslib(1,"./benchmark/datasets/siftsmall/siftsmall");
    benchmark_searchfaisslib(1, "./benchmark/datasets/siftsmall/siftsmall");
   
    benchmark_insertion_faisslib(2,"./benchmark/datasets/sift/sift");
    benchmark_searchfaisslib(2, "./benchmark/datasets/sift/sift");

}
