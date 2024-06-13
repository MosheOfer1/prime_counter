#ifndef NODE_POOL_H
#define NODE_POOL_H

// Forward declaration of Node
struct Node;

#define NODE_POOL_SIZE 100000000

typedef struct {
    struct Node* free_list; // Pointer to the head of the free list
    struct Node* nodes;     // Pointer to the array of nodes in the pool
    int max_nodes;          // Maximum number of nodes in the pool
    pthread_mutex_t mutex; 
    pthread_cond_t cond;
} NodePool;

NodePool* create_node_pool(int max_nodes);
struct Node* allocate_node(NodePool* pool);
void free_node(NodePool* pool, struct Node* node);
void destroy_node_pool(NodePool* pool);

#endif /* NODE_POOL_H */
