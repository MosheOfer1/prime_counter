#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include "MSqueue.h"

// Thread pool structure
typedef struct {
    pthread_t* threads;
    int number_of_threads;
    Queue* task_queue;
    _Atomic bool stop;
    int* true_counts;
    bool (*function)(int);
} ThreadPool;

// Struct to pass arguments to worker threads
typedef struct {
    int thread_index;
    ThreadPool* pool;
} ThreadArg;

// Function to initialize the thread pool
ThreadPool* thread_pool_init(int thread_count, bool (*function)(int));

// Function to add a task to the thread pool
void thread_pool_add_task(ThreadPool* pool, int* nums);

// Function to destroy the thread pool
int thread_pool_destroy(ThreadPool* pool);

#endif // THREAD_POOL_H
