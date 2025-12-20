#ifndef HNSW_HEAP_H
#define HNSW_HEAP_H
#include "vtraceCommon.h"

typedef struct leafNode leafNode;

 struct leafNode{
    int id;

    void* data;
    leafNode* right;
    leafNode* left;

};


typedef struct{
    leafNode* root;
    uint32 size;
} BinaryTree;

extern BinaryTree* initTree();
extern void disposeTree(BinaryTree* tree);
extern void insertNodeIntoTree(BinaryTree* tree, void* data);


#endif