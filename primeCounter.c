#include <stdio.h>
#include <stdbool.h>
#include <time.h> 
#include <stdlib.h>
#include "thread_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#define DFAULT_CHUNK_SIZE 100
#define DFAULT_WORKERS 2

int get_num_cores() {
    return sysconf(_SC_NPROCESSORS_ONLN);
}

// Function to perform modular exponentiation
// It returns (base^exp) % mod
unsigned long long mod_exp(unsigned long long base, unsigned long long exp, unsigned long long mod) {
    unsigned long long result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) {  // If exp is odd, multiply base with result
            result = (result * base) % mod;
        }
        exp = exp >> 1;      // exp = exp / 2
        base = (base * base) % mod;  // base = base^2 % mod
    }
    return result;
}

// Miller-Rabin test for a single round
bool miller_rabin_test(unsigned long long d, unsigned long long n) {
    // Pick a random number in [2, n-2]
    // Corner cases: n > 4
    unsigned long long a = 2 + rand() % (n - 4);
    
    // Compute a^d % n
    unsigned long long x = mod_exp(a, d, n);
    
    if (x == 1 || x == n - 1) {
        return true;
    }

    // Keep squaring x while one of the following doesn't happen:
    // (i) d does not reach n-1
    // (ii) (x^2) % n is not 1
    // (iii) (x^2) % n is not n-1
    while (d != n - 1) {
        x = (x * x) % n;
        d *= 2;

        if (x == 1) {
            return false;
        }
        if (x == n - 1) {
            return true;
        }
    }
    return false;
}

// Miller-Rabin primality test
bool isPrime(int n) {
    // Number of iterations (adjust this value for more or less accuracy)
    int k = 5;  // This is a good balance between accuracy and performance

    // // Corner cases
    if (n <= 1 || n == 4) {
        return false;
    }
    if (n <= 3) {
        return true;
    }

    // Find d such that n-1 = d * 2^r for some r >= 1
    unsigned long long d = n - 1;
    while (d % 2 == 0) {
        d /= 2;
    }

    // Iterate given number of 'k' times
    for (int i = 0; i < k; i++) {
        if (!miller_rabin_test(d, n)) {
            return false;
        }
    }

    return true;
}

int main(int argc, char *argv[]) {
    int total_counter = 0;
    int num_of_workers = get_num_cores() - 1;
    int chunk_size = DFAULT_CHUNK_SIZE;
    
    // If a command-line argument is provided, use it as the number of workers
    if (argc > 1) {
        num_of_workers = atoi(argv[1]);
        if (num_of_workers <= 0) {
            fprintf(stderr, "Invalid number of workers specified. Using default: %d\n", DFAULT_WORKERS);
            num_of_workers = DFAULT_WORKERS;
        }
    }
    if (argc > 2) {
        chunk_size = atoi(argv[2]);
        if (chunk_size <= 0) {
            fprintf(stderr, "Invalid number of chunk size specified. Using default: %d\n", DFAULT_CHUNK_SIZE);
            chunk_size = DFAULT_CHUNK_SIZE;
        }
    }
    
    ThreadPool *pool = thread_pool_init(num_of_workers, isPrime);

    // Read numbers from stdin until end of file
    while (true) {
        int *nums = (int *)calloc(chunk_size, sizeof(int));
        if (nums == NULL) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        
        int count = 0;
        while (count < chunk_size && scanf("%d", &nums[count]) != EOF) {
            count++;
        }

        if (count == 0) {
            free(nums);
            break;
        }

        int *task = (int *)calloc(count + 1, sizeof(int));
        if (task == NULL) {
            perror("Failed to allocate memory for task");
            free(nums);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < count; i++) {
            task[i] = nums[i];
        }
        task[count] = -1;

        thread_pool_add_task(pool, task);
        free(nums);
    }

    total_counter = thread_pool_destroy(pool); // Shutdown gracefully

    printf("%d total primes.\n", total_counter);

    return 0;
}
