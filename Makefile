
CC := gcc
OPTIMIZATION_FLAGS := -ffast-math -O3
FLAGS := -I /Users/leon/code/hnsw/inc -pg
BIN := vtrace
DEBUG_FLAGS := -fsanitize=address -fsanitize=undefined -Wall -Wextra -O0 -g
NAME := vtrace

all: debug

release:
	$(CC) ./src/*.c $(FLAGS) $(OPTIMIZATION_FLAGS) -o ./bin/$(NAME)

benchmark:
	$(CC) ./src/*.c ./benchmark/benchmark.c $(FLAGS) $(OPTIMIZATION_FLAGS) -o ./bin/$(NAME)
debug:
	$(CC) ./src/*.c $(DEBUG_FLAGS) $(FLAGS) -o ./bin/$(NAME)

lib:
	$(CC) ./src/*.c -shared -fPIC -fvisibility=hidden -O3 -ffast-math -o bin/hnsw.so -I /Users/leon/code/hnsw/inc


clean:
	rm -rf ./bin/*


