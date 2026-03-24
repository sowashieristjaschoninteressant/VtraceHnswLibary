
CC := gcc
HEADERS := -I ./inc
LIB_FLAGS := $(HEADERS) -shared -fPIC -fvisibility=hidden

OPTIMIZATION_FLAGS := -ffast-math -O3 -march=native -ffast-math -flto -lm -mavx2 -mfma
DEBUG_FLAGS :=  -Wall -Wextra -O0 -g -lm -mavx2 -mfma
NAME := libvtrace.so

.PHONY: all debug release benchmark debugTest clean

all: debug

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


