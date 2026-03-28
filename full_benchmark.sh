#! /usr/bin/env bash
export OMP_NUM_THREADS=1
export OPENBLAS_NUM_THREADS=1
export MKL_NUM_THREADS=1
#=== build libs usw.====
make clean;

make benchmark;
make hnswlib_bench;
make faiss_bench;

./bin/bench
./bin/bench_hnswlib
./bin/faiss_bench



unset OMP_NUM_THREADS;