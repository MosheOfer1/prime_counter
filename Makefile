.PHONY: all clean

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -g

# Target binaries
TARGETS = randomGenerator primeCounter naive

all: $(TARGETS)

randomGenerator: generator.c
	$(CC) $(CFLAGS) -o randomGenerator generator.c

primeCounter: primeCounter.c thread_pool.c thread_pool.h MSqueue.c MSqueue.h hazard_pointer.h hazard_pointer.c
	$(CC) $(CFLAGS) -o primeCounter primeCounter.c thread_pool.c MSqueue.c hazard_pointer.c

naive: primeCounter_naive.c
	$(CC) $(CFLAGS) -o naive primeCounter_naive.c

clean:
	rm -f $(TARGETS)
