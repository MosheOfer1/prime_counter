#ifndef MS_QUEUE_H
#define MS_QUEUE_H

#include <stdatomic.h>
#include <stdbool.h>
#include <pthread.h>
#include "hazard_pointer.h"

// #include "node_pool.h"

// This is what limiting the memory usage to 2MB
#define QUEUE_MAX_SIZE 2000

// Queue structure with _Atomic qualified head and tail pointers
typedef struct Queue {
    pthread_mutex_t size_mutex;
    pthread_cond_t size_cond;
    _Atomic(int) size;
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
} Queue;

// Function to create a new node
Node* new_node(Queue* queue, int *data);

// Function to initialize a new queue
Queue* new_queue(int number_of_threads);

// Function to enqueue an element
void enqueue(Queue* queue, int *data);

// Function to dequeue an element
int *dequeue(Queue* queue, int thread_index);

// Function to clean up the queue
void free_queue(Queue* queue);

#endif // MS_QUEUE_H
