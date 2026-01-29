
CC := gcc
OPTIMIZATION_FLAGS := -ffast-math -O3
FLAGS := -I /Users/leon/code/hnsw/inc -pg
BIN := vtrace
DEBUG_FLAGS := -fsanitize=address -fsanitize=undefined -Wall -Wextra -O0 -g
NAME := vtrace

all: debug

release:
	$(CC) ./src/*.c $(FLAGS) $(OPTIMIZATION_FLAGS) -o ./bin/$(NAME)

debug:
	$(CC) ./src/*.c $(DEBUG_FLAGS) $(FLAGS) -o ./bin/$(NAME)


clean:
	rm -rf ./bin/*


