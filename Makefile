
CC := gcc
HEADERS := -I /Users/leon/code/hnsw/inc
LIB_FLAGS := $(HEADERS) -shared -fPIC -fvisibility=hidden

OPTIMIZATION_FLAGS := -ffast-math -O3 -march=native -ffast-math -flto
DEBUG_FLAGS := -fsanitize=address -fsanitize=undefined -Wall -Wextra -O0 -g
NAME := libvtrace.so

all: debug

release:
	$(CC) ./src/*.c $(LIB_FLAGS) $(OPTIMIZATION_FLAGS) -o ./bin/$(NAME)

benchmark: release
	$(CC)  ./benchmark/benchmark.c -L./bin -lvtrace $(HEADERS) $(OPTIMIZATION_FLAGS) -o ./bin/bench

debug:
	$(CC) ./src/*.c $(DEBUG_FLAGS) $(LIB_FLAGS)  -o ./bin/$(NAME)

debugTest: debug
	$(CC) ./debug/debug.c -L./bin -lvtrace $(HEADERS) $(DEBUG_FLAGS) -o ./bin/debug

clean:
	rm -rf ./bin/*


