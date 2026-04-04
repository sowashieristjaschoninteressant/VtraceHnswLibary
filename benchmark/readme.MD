##### benchmarks 


#### Datasets

the datasets are all derived from http://corpus-texmex.irisa.fr/ and can be easily downloaded.

in usage is currently;
- siftsmall ( 10000 vectores, 128 dimensions)
- sift (1M vectores, 128 dimensions)
- gist (1M vectores, 960 dimensions)


### how are the benchmarks structured?

(IMPORTANT! the datasets will not be inside this project! Also only tested on macos sonoma M1 14.2.1 (23C71) and Windows 10, Ubuntu WSL )
well, firstly every libary gets compiled into a shared Object file and then linked as an DLL to a benchmark.c file. For the C++ implementations a little wrapper was needed to be able to compile with a c compiler and ensure the same compilation structure. every lib has its own benchmark file that gets compiled via the make command and the makefile. Its important that u are in the same directory as the makefile for this to work. The compiled binary will then moved to /bin.
As of now i only tested executing the binary from the project folder root. If tried otherwise it could be possible that the hardcoded paths dont find the datasets.


### how to reproduce the benchmarks?

1. Download the siftsmall, sift and gist dataset from http://corpus-texmex.irisa.fr/ and put them into the /datasets directory

2. decompress them using tar -xf

3. 