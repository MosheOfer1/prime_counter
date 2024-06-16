#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include "MSqueue.h"

HazardPointer hp;

// Create a new node
Node *new_node(Queue *queue, int *data)
{
    int size = atomic_fetch_add(&queue->size, 1);
    if (size == QUEUE_MAX_SIZE)
    {
        pthread_mutex_lock(&queue->size_mutex);
        // printf("reached max tasks in the queue %d\n", size);
        pthread_cond_wait(&queue->size_cond, &queue->size_mutex);
        pthread_mutex_unlock(&queue->size_mutex);
    }

    Node *node = (Node *)malloc(sizeof(Node));
    // Node* node = allocate_node(queue->node_pool);
    if (!node)  
    {
        perror("Failed to allocate node");
        exit(EXIT_FAILURE);
    }
    node->data = data;
    atomic_init(&node->next, NULL);
    return node;
}

// Initialize a new queue
Queue *new_queue(int number_of_threads)
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (!queue)
    {
        perror("Failed to allocate queue");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&queue->size_mutex, NULL) != 0) {
        // Handle initialization failure
        perror("Failed to initialize mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&queue->size_cond, NULL) != 0) {
        // Handle initialization failure
        perror("Failed to initialize condition variable");
        exit(EXIT_FAILURE);
    }
    atomic_init(&queue->size, 1);
    Node *dummy = new_node(queue, 0);
    atomic_init(&queue->head, dummy);
    atomic_init(&queue->tail, dummy);
    init_hazard_pointers(&hp, number_of_threads);
    return queue;
}

// This function done only by a single thread
void enqueue(Queue *queue, int *data)
{
    Node *node = new_node(queue, data);
    Node *tail = queue->tail;

    tail->next = node;  // Link the new node to the current tail
    queue->tail = node; // Update the tail to the new node
}

// Dequeue an element (thread-safe)
int *dequeue(Queue *queue, int thread_index)
{
    Node *head;
    Node *tail;
    Node *next;
    int *data;

    while (true) {
        head = atomic_load(&queue->head);
        hp.pointers[thread_index][0] = head;
        if (head != atomic_load(&queue->head)){
            continue;
        }
        tail = atomic_load(&queue->tail);

        next = atomic_load(&head->next);
        hp.pointers[thread_index][1] = next;
        if (head == atomic_load(&queue->head)) {
            if (head == tail) {
                if (next == NULL) {
                    return NULL; // Queue is empty
                }
                atomic_compare_exchange_strong(&queue->tail, &tail, next);
            }
            else {
                data = next->data;
                if (atomic_compare_exchange_strong(&queue->head, &head, next)) {
                    // Decrement the size atomically
                    int size = atomic_fetch_sub(&queue->size, 1);
                    if (size == QUEUE_MAX_SIZE / 3) {
                        // Only one thread will enter and signal to the main thread to continue enqueueing
                        pthread_mutex_lock(&queue->size_mutex);
                        pthread_cond_signal(&queue->size_cond);
                        pthread_mutex_unlock(&queue->size_mutex);
                    }

                    // Release the old head node
                    retire_node(&hp, head);
                    return data;
                }
            }
        }   
    }
}


// Clean up the queue
void free_queue(Queue *queue)
{
    // Final scan to free any remaining retired nodes
    scan_retired_nodes(&hp);
    free(queue);
}
