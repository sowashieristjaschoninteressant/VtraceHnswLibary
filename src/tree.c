#include "tree.h"


BinaryTree* initTree(){

    BinaryTree* tree = malloc(sizeof(BinaryTree));
    assert(tree);

    tree->size = 0;
    tree->root = NULL;

    return tree;
}


void disposeTree(BinaryTree* tree){

    // i will also need to free() the leafnodes but that algorithm will come later


    free(tree);

}


void insertNodeIntoTree(BinaryTree* tree, void* data){

    
    leafNode* node = malloc(sizeof(leafNode));
    assert(node);
    // NOTE: so as of right now we asume the data this pointer points to is aleready in the heap
    node->data = data; 
    node->left = NULL;
    node->right = NULL;

    tree->size++;

    if( tree->root == NULL){
        tree->root = node;
        return;
    }

    uint32 back,front;
    back=front =0;

    leafNode* queue[1024];
    queue[back++] = tree->root;

    while(back > front){
        leafNode* cur = queue[front++];

        if(cur->left == NULL){
            cur->left = node;
            return;
        }
        if(cur->right == NULL){
            cur->right = node;
            return;
        }

       queue[back++] = cur->left;
       queue[back++] = cur->right;
    }

}