#include "thread_pool.h"
#include <stdlib.h>
#include <stdio.h>

// Worker thread function
void* worker_thread(void* arg) {
    ThreadArg* thread_arg = (ThreadArg*)arg;
    int thread_index = thread_arg->thread_index;
    ThreadPool* pool = thread_arg->pool;
    free(thread_arg);
    
    while (true) {
        int num = dequeue(pool->task_queue, thread_index);
        if (atomic_load(&pool->stop) && num == -1) {
            if (atomic_load(&pool->task_queue->head) == atomic_load(&pool->task_queue->tail)) 
                break;  // Exit if the queue is empty and stop flag is set
        }

        if (num != -1) {
            if (pool->function(num)) {
                pool->true_counts[thread_index]++;
            }   
        }
        
    }  
    free_all_retired_nodes();
    return NULL;
}

// Initialize the thread pool
ThreadPool* thread_pool_init(int number_of_threads, bool (*function)(int)) {
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    if (!pool) {
        perror("Failed to allocate thread pool");
        exit(EXIT_FAILURE);
    }
    pool->number_of_threads = number_of_threads;
   
    pool->function = function;
    pool->task_queue = new_queue(number_of_threads);
    atomic_init(&pool->stop, false);
    pool->true_counts = (int*)malloc(number_of_threads * sizeof(int));

    pool->threads = (pthread_t*)malloc(number_of_threads * sizeof(pthread_t));
    if (!pool->threads) {
        perror("Failed to allocate threads");
        free(pool);
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < number_of_threads; ++i) {
        pool->true_counts[i] = 0;
        ThreadArg* arg = (ThreadArg*)malloc(sizeof(ThreadArg));
        if (!arg) {
            perror("Failed to allocate thread argument");
            exit(EXIT_FAILURE);
        }
        arg->thread_index = i;
        arg->pool = pool;
        if (pthread_create(&pool->threads[i], NULL, worker_thread, arg) != 0) {
            perror("Failed to create thread");
            free(pool->threads);
            free(pool);
            exit(EXIT_FAILURE);
        }
    }
    return pool;
}

// Add a task to the thread pool
void thread_pool_add_task(ThreadPool* pool, int num) {
    enqueue(pool->task_queue, num);
}

// Destroy the thread pool
int thread_pool_destroy(ThreadPool* pool) {
    atomic_store(&pool->stop, true);
    
    for (int i = 0; i < pool->number_of_threads; ++i) {
        pthread_join(pool->threads[i], NULL);
    }
    int sum = 0;
    for (int i = 0; i < pool->number_of_threads; i++) {
        sum += pool->true_counts[i];
        // printf("%d\n", pool->true_counts[i]);
    }
    free(pool->threads);
    free_queue(pool->task_queue);
    free(pool->true_counts);
    free(pool);

    return sum;
}

