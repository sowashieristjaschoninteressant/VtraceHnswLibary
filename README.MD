# vtrace – Approximate Nearest Neighbor

`vtrace` is a high-performance C-based library and benchmarking framework for evaluating Approximate Nearest Neighbor (ANN) search algorithms.
It includes a custom HNSW implementation and provides direct comparisons against FAISS and hnswlib via lightweight C/C++ wrappers.

---

##  Features

* Custom **HNSW implementation (vtrace)**
* Benchmarks against:

  * FAISS
  * hnswlib
* Supports:

  * Insertion benchmarks
  * Search benchmarks
  * Recall@K evaluation
  * QPS (queries per second)
  * Latency measurement
* CSV output for plotting and analysis
* Optimized with AVX2, FMA, and aggressive compiler flags

---

##  Project Structure

```
.
├── src/                # vtrace core implementation
├── inc/                # headers
├── benchmark/          # benchmark code + datasets
│   ├── datasets/       # fvecs datasets (e.g. SIFT, GIST)
│   ├── faiss_wrapper.cpp
│   ├── hnswlib_wrapper.cpp
│   ├── benchmark.c
│   ├── benchmark_faiss.c
│   └── benchmark_hnswlib.c
├── debug/              # debug utilities
├── bin/                # compiled binaries
├── Makefile
└── README.md
```


---

##  Using vtrace as a Library

The `vtrace` library provides a simple C API for building and querying an HNSW index.

### 1. Compile the shared object via the makefile 'make release' command

### 2. Include the header

```c
#include "hnsw_public.h"
```

---

### 3. Create an index

```c
int ef_construction = 200;
HNSW* index = hnsw_init(ef_construction);
```

---

### 4. Insert vectors

```c
int M = 12;

vec v;
v.dim = dim;
v.vec = your_float_array;

hnsw_insert(index, &v, M);
```

Insert all vectors sequentially:

```c
for (int i = 0; i < N; i++) {
    v.vec = data + i * dim;
    hnsw_insert(index, &v, M);
}
```

---

### 5. Configure search

```c
int ef_search = 100;
hnsw_set_ef(index, ef_search);
```

---

### 6. Perform search

```c
int K = 10;
hnswResult result;

hnsw_search(index, &v, K, &result);
```

Access results:

```c
for (int i = 0; i < K; i++) {
    int id = result.ids[i];
    // process result
}
```

---

### 7. Free resources

```c
hnsw_free(index);
```

---

## 🧠 Data Structure

```c
typedef struct {
    int dim;
    float* vec;
} vec;
```

---

## ⚡ Minimal Example

```c
#include "hnsw_public.h"

int main() {
    int dim = 128;
    int N = 1000;

    HNSW* index = hnsw_init(200);

    vec v;
    v.dim = dim;

    // insert random data
    for (int i = 0; i < N; i++) {
        v.vec = generate_vector(dim); // user-defined
        hnsw_insert(index, &v, 12);
    }

    // query
    v.vec = generate_vector(dim);
    hnsw_set_ef(index, 100);

    hnswResult result;
    hnsw_search(index, &v, 10, &result);

    hnsw_free(index);
}
```

---

## ⚠️ Notes

* All vectors must have the same dimensionality
* Memory for input vectors is managed by the user
* `ef_construction` impacts build quality vs speed
* `ef_search` controls recall vs latency trade-off

---

---

##  Build

### Debug build

```bash
make debug
```

### Release build (optimized)

```bash
make release
```

### Clean

```bash
make clean
```

---

##  Benchmarks

### vtrace benchmark

```bash
make benchmark
./bin/bench
```

### FAISS benchmark

```bash
make faiss_bench
./bin/bench_faiss
```

### hnswlib benchmark

```bash
make hnswlib_bench
./bin/bench_hnswlib
```

---

##  Output

All benchmarks generate CSV files:

### Insertion

```
dimension,M,ef_construction,build_time_ms,avg_insert_us,vec_count
```

### Search

```
N,dimension,M,ef_search,avg_latency_ms,qps,recall,numQueries
```

These can be directly used for plotting (e.g. Python, matplotlib, gnuplot).

---

##  Dataset Format

The framework uses `.fvecs` format:

* First 4 bytes: dimension (int)
* Followed by float32 vector data
* Repeated for all vectors

Example datasets:

* SIFT
* GIST

---

##  Optimization Flags

Release builds use:

```
-O3 -march=native -mavx2 -mfma -ffast-math -flto
```

⚠️ Note:
`-ffast-math` may slightly affect numerical accuracy and recall.

---

## 📏 Benchmark Methodology

* Warmup phase before measurements
* Multiple `ef_search` values tested
* Recall computed against brute-force ground truth
* Metrics:

  * Average latency per query
  * QPS
  * Recall@K

---

##  Notes

* AVX2 is enabled via compiler flags (`-march=native`)
* Benchmarks assume consistent vector dimensionality
* Ground truth is computed using:

  * Linear scan (vtrace)
  * FAISS flat index
  * hnswlib brute-force index

---

##  Requirements

* GCC / G++
* OpenMP
* BLAS (for FAISS)
* FAISS (installed system-wide)
* hnswlib (included or header-only)

---

##  Future Work

* Multi-threaded benchmarking
* GPU support (FAISS)
* Additional ANN algorithms
* Automated plotting pipeline
* Reproducibility tooling (fixed seeds, CPU pinning)

---

##  License

MIT (or specify your license here)

---

##  Author

Leon Margale – BA. Effiziente Suche und Indexierung in Vektorräumen

---

##  Example

```bash
make faiss_bench
./bin/bench_faiss
```

Output:

```
faiss_benchmark_search_2_...csv
```

---

This project is intended for research and performance evaluation of ANN systems.
