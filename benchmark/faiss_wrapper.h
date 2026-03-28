typedef struct FaissIndex FaissIndex;
typedef struct FaissIndexFlat FaissIndexFlat;

FaissIndex* faiss_init(int dim, int M, int ef_construction);
void faiss_add(FaissIndex* idx, float*data, int n);
void faiss_set_ef(FaissIndex* idx, int ef);
void faiss_search(FaissIndex* idx, float* data, int K, int* out_ids);
void faiss_free(FaissIndex* idx);

FaissIndexFlat* faiss_bf_init(int dim);
void faiss_bf_add(FaissIndexFlat* idx, int size, float* vecs);
void faiss_linear(FaissIndexFlat* idx, float* query, int K, int* ids);
void faiss_bf_free(FaissIndexFlat* idx);