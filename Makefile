CC := gcc
HEADERS := -I ./inc
LIB_FLAGS := $(HEADERS) -shared -fPIC -fvisibility=hidden

OPTIMIZATION_FLAGS := -ffast-math -O3 -march=native -ffast-math -flto -lm -mavx2 -mfma -D__AVX2__
DEBUG_FLAGS :=  -Wall -Wextra -O0 -g -lm -march=native -mavx2 -mfma -D__AVX2__
NAME := libvtrace.so

.PHONY: all debug release benchmark debugTest clean

all: debug

faiss_wrapper:
	g++ ./benchmark/faiss_wrapper.cpp \
		-shared -fPIC \
		-lfaiss -lopenblas -lstdc++ -fopenmp -lm   \
		-O3 -march=native -mavx2 -mfma -ffast-math \
		-o ./bin/libfaiss_wrapper.so

faiss_bench: faiss_wrapper
	gcc ./benchmark/benchmark_faiss.c \
	-I ./benchmark \
	-L ./bin -lfaiss_wrapper \
	-Wl,-rpath,'$$ORIGIN/../bin' \
	-O3 -march=native -mavx2 -mfma -ffast-math -lm \
	-o ./bin/bench_faiss


hnswlib_wrapper:
	g++ ./benchmark/hnswlib_wrapper.cpp \
		-I ./benchmark/hnswlib \
		-shared -fPIC \
		-O3 -march=native -mavx2 -mfma -ffast-math \
		-o ./bin/libhnswlib_wrapper.so

hnswlib_bench: hnswlib_wrapper
	gcc ./benchmark/benchmark_hnswlib.c \
		-I ./benchmark \
		-L ./bin -lhnswlib_wrapper \
		-Wl,-rpath,'$$ORIGIN/../bin' \
		-O3 -march=native -mavx2 -mfma -ffast-math -lm \
		-o ./bin/bench_hnswlib

release:
	$(CC) ./src/*.c $(LIB_FLAGS) $(OPTIMIZATION_FLAGS) -o ./bin/$(NAME)

benchmark: release
	$(CC)  ./benchmark/benchmark.c -L./bin -lvtrace $(HEADERS) $(OPTIMIZATION_FLAGS) -o ./bin/bench


debugTest: debug
	$(CC) ./debug/debug.c -L./bin -lvtrace $(HEADERS) $(DEBUG_FLAGS) -o ./bin/debug

clean:
	rm -rf ./bin/*

debug:
	$(CC) ./src/*.c $(LIB_FLAGS) $(DEBUG_FLAGS) -o ./bin/$(NAME)
