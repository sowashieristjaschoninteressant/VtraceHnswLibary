#ifndef VTRACE_H
#define VTRACE_H
/**
 * @file vtrace.h
 * @author Leon Margale (leon@margale.de)
 * @brief 
 * @version 0.1
 * @date 2025-12-11
 * 
 * @copyright Copyright (c) 2025
 * 
 * Vtrace is a vector search Libary, that implements the HNSW (similarity search in vector spaces using small hirachical Worlds) that gets developed for a bachlor thesis.
 * Vtrace tries to optimise the hnsw algorithm using the following techniques: Memory Layout structure, smid operations, L1 cache misses reduction.  
 */


 #include "stdlib.h"
 #include "stdio.h"
 #include "fcntl.h"
 #include "math.h"
 #include "time.h"
 

typedef unsigned char     uint8;
typedef unsigned short    uint16;
typedef unsigned int      uint32;
typedef unsigned long int uint64;

typedef char     int8;
typedef short    int16;
typedef int      int32;
typedef long int int64;


typedef struct {
  uint32 id;
  uint32 maxLevel;
  uint32** neigbours;
  void* data;
} node;

typedef node VT_node;

typedef struct {
  uint32 efsearch;
  uint32 efconstruction;
  uint32 maxLayer;
  uint32 size;
  node* nodes;

  node* entrypoint;
  
} Graph;

typedef Graph VT_graph;

extern VT_graph VTcreateGraph(void* data, uint32 size);
extern float VTsearch(Graph*, float vec);
extern void VTinsert(Graph*, float vec);
// sample level
extern int levelSample(uint32,float);

#endif