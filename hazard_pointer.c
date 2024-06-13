/**
 * I based my code on this article:
 * Hazard Pointers: Safe Memory Reclamation for
 * Lock-Free Objects
 * Maged M. Michael 
 */
#include "hazard_pointer.h"
#include <stdio.h>


// Thread-local storage for retired count and list
static __thread int retired_count = 0;

typedef struct RetiredNode {
    Node* node;
    struct RetiredNode* next;
} RetiredNode;

static __thread RetiredNode* retired_list = NULL;

void init_hazard_pointers(HazardPointer* hp, int max_threads) {
    hp->max_threads = max_threads;
    hp->pointers = (void***)malloc(max_threads * sizeof(void**));
    for (int i = 0; i < max_threads; ++i) {
        hp->pointers[i] = (void**)malloc(MAX_HAZARD_POINTERS * sizeof(void*));
        for (int j = 0; j < MAX_HAZARD_POINTERS; ++j) {
            hp->pointers[i][j] = NULL;
        }
    }
}

void free_hazard_pointers(HazardPointer* hp) {
    if (hp == NULL) {
        return;
    }
    
    for (int i = 0; i < hp->max_threads; ++i) {
        free(hp->pointers[i]);
    }
    free(hp->pointers);
    hp->pointers = NULL;
}

bool is_hazard_pointer(HazardPointer* hp, void* ptr) {
    for (int i = 0; i < hp->max_threads; ++i) {
        for (int j = 0; j < MAX_HAZARD_POINTERS; ++j) {
            if (hp->pointers[i][j] == ptr) {
                return true;
            }
        }
    }
    return false;
}

void push_retired_node(Node* node) {
    RetiredNode* retired_node = (RetiredNode*)malloc(sizeof(RetiredNode));
    retired_node->node = node;
    retired_node->next = retired_list;
    retired_list = retired_node;
    retired_count++;
}

void retire_node(HazardPointer* hp, Node* node) {
    push_retired_node(node);

    if (retired_count >= RETIRE_THRESHOLD) {
        scan_retired_nodes(hp);
    }
}

void scan_retired_nodes(HazardPointer* hp) {
    RetiredNode* current = retired_list;
    RetiredNode* prev = NULL;

    while (current != NULL) {
        if (!is_hazard_pointer(hp, current->node)) {
            // Safe to free the node
            if (prev == NULL) {
                retired_list = current->next;
            } else {
                prev->next = current->next;
            }
            RetiredNode* to_free = current;
            current = current->next;
            free(to_free->node);
            free(to_free);
            retired_count--;
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void free_all_retired_nodes() {
    RetiredNode* current = retired_list;
    while (current != NULL) {
        RetiredNode* to_free = current;
        current = current->next;
        free(to_free->node);
        free(to_free);
    }
    retired_list = NULL;
    retired_count = 0;
}

