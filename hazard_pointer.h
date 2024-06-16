#ifndef HAZARD_POINTER_H
#define HAZARD_POINTER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_HAZARD_POINTERS 2
#define RETIRE_THRESHOLD 10

typedef struct HazardPointer {
    void*** pointers;
    int max_threads;
} HazardPointer;

// Node structure with _Atomic qualified next pointer
typedef struct Node {
    int *data;
    _Atomic(struct Node*) next;
} Node;

void init_hazard_pointers(HazardPointer* hp, int max_threads);
void free_hazard_pointers(HazardPointer* hp);
void free_all_retired_nodes();
bool is_hazard_pointer(HazardPointer* hp, void* ptr);
void retire_node(HazardPointer* hp, Node* node);
void scan_retired_nodes(HazardPointer* hp);

#endif // HAZARD_POINTER_H
