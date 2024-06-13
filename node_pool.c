#include <stdlib.h>
#include <stdio.h>
#include "MSqueue.h"
#include "node_pool.h"

// Initialize the node pool
NodePool* create_node_pool(int max_nodes) {
    NodePool* pool = (NodePool*)malloc(sizeof(NodePool));
    if (!pool) {
        perror("Failed to allocate node pool");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the array of nodes
    pool->nodes = (Node*)malloc(max_nodes * sizeof(Node));
    if (!pool->nodes) {
        perror("Failed to allocate memory for node pool");
        free(pool);
        exit(EXIT_FAILURE);
    }

    // Initialize the mutex and condition variable
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);

    // Initialize the free list
    pool->free_list = NULL;
    pool->max_nodes = max_nodes;

    // Populate the free list with nodes
    for (int i = 0; i < max_nodes; i++) {
        pool->nodes[i].next = pool->free_list;
        pool->free_list = &pool->nodes[i];
    }

    return pool;
}

// Allocate a node from the pool
Node* allocate_node(NodePool* pool) {
    pthread_mutex_lock(&pool->mutex);

    // Wait until a node is available
    while (pool->free_list == NULL) {
        pthread_cond_wait(&pool->cond, &pool->mutex);
        //printf("c");
    }

    // Pop a node from the free list
    Node* node = pool->free_list;
    pool->free_list = node->next;

    pthread_mutex_unlock(&pool->mutex);
    return node;
}

// Free a node and return it to the pool
void free_node(NodePool* pool, Node* node) {
    pthread_mutex_lock(&pool->mutex);

    // Push the node back to the free list
    node->next = pool->free_list;
    pool->free_list = node;

    // Signal that a node has been freed
    pthread_cond_signal(&pool->cond);

    pthread_mutex_unlock(&pool->mutex);
}

// Cleanup the node pool
void destroy_node_pool(NodePool* pool) {
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
    free(pool->nodes); 
    free(pool);  
}     