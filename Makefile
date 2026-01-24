
CC := gcc
FLAGS := -Wall -Wextra -O0 -g -ffast-math -I /Users/leon/code/hnsw/inc -pg
BIN := vtrace

all: vtrace


vtrace:
	$(CC) ./src/*.c $(FLAGS) -o ./bin/vtrace

clean:
	rm -rf ./bin/*


