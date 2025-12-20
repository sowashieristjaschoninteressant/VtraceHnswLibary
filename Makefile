
CC := gcc
FLAGS := -Wall -Wextra -O3 -ffast-math -I /Users/leon/code/hnsw/inc
BIN := vtrace

all: vtrace


vtrace:
	$(CC) ./src/*.c $(FLAGS) -o ./bin/vtrace

clean:
	rm ./bin/*


