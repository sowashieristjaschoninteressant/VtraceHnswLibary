#ifndef HNSW_ALLOC_H
#define HNSW_ALLOC_H

#include "vtraceCommon.h"

void* hnsw_alloc(size_t size);
void hnsw_free(void* ptr);




#endif