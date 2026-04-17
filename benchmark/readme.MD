##### benchmarks 
### hardware specifiacations

these benchmarks got tested on 
- macos sonoma M1 14.2.1 (23C71) (2020)


the current csv files are from wsl2 ubuntu with these hardware specs

- Windows 10 Version 22H2 (OS Build 19045.6466)
    Device Name	DESKTOP-KN3E9UA
    Processor	Intel(R) Core(TM) i7-4790 CPU @ 3.60GHz   3.60 GHz
    Installed RAM	28,0 GB (27,9 GB usable)
    Storage	238 GB SSD SanDisk SDSSDHP256G, 932 GB HDD WDC WD10EZEX-60M2NA0
    Graphics Card	NVIDIA GeForce GTX 1050 Ti (4 GB), Intel(R) HD Graphics 4600 (113 MB)
    Device ID	14BD5E15-7FB3-49D3-B1DA-F80B59F39E2D
    System Type	64-bit operating system, x64-based processor
    Pen and touch	No pen or touch input is available for this display

- WSL2 ubuntu

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

1. regarding on wich platform you are right now, change the dll name (NAME variable ) in the makefile to end with .so for maxos/linux and .dll for windows.

2. Download the siftsmall, sift and gist dataset from http://corpus-texmex.irisa.fr/ and put them into the /datasets directory. Download and configure the 2 libarys hnswlib (clone it into the empty hnswlib directory here) and faiss

3. decompress the datasets using tar -xf

(IMPORTANT! you can adjust the dataset u want to test in every benchmark.c file in the main method. just update the path parameter to point to the dataset of your choice. U can just insert the path to the directory, where all dataset files are that is enough the function will handle everything else)

4. if u have everything set up it should be possible to just execute the full_benchmark.sh script to compile the libarys and execute them.

5. these benchmarks produce csv's the naming might look weird but for now its like that to make sure they dont overwrite themselfs. This csv's should be put into the csv directory and then its possible to plot them via the 2 pythonscripts.