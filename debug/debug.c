#include "stdio.h"

#include "hnsw_public.h"

int main(void){

    const int ef = 20;
    HNSW* h = hnsw_init(ef);


    return 0;
}