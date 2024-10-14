

#include "malloc_mock.h"


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


// a simple DynArray, to not use stb_ds.h

typedef struct {
    void* ptr;
    size_t new_size;
} DynArrayRNode;

typedef struct {
    uint64_t size;
    DynArrayRNode* nodes;
} DynArrayR;


typedef struct {
    void* ptr;
} DynArrayFNode;

typedef struct {
    uint64_t size;
    DynArrayFNode* nodes;
} DynArrayF;


struct MockMallocStatsImpl {
    // call history
    DynArrayR* realloced_calls;
    DynArrayF* free_calls;
    // free fail history
    size_t realloc_fail_count;
};


static MockMallocStats* global_stats = NULL;


void mock_free(void* ptr) {
    assert(global_stats);

    size_t previous_size = global_stats->free_calls->size;

    global_stats->free_calls->size++;

    global_stats->free_calls->nodes =
            realloc(global_stats->free_calls->nodes,
                    global_stats->free_calls->size * sizeof(*global_stats->free_calls->nodes));

    global_stats->free_calls->nodes[previous_size].ptr = ptr;

    free(ptr);
}

void* mock_realloc(void* ptr, size_t new_size) {
    assert(global_stats);

    fprintf(stderr, "TEST here %lu\n", global_stats->realloc_fail_count);
    exit(4);
    if (global_stats->realloc_fail_count > 0) {
        global_stats->realloc_fail_count--;
        return NULL;
    }

    size_t previous_size = global_stats->realloced_calls->size;

    global_stats->realloced_calls->size++;

    global_stats->realloced_calls->nodes =
            realloc(global_stats->realloced_calls->nodes,
                    global_stats->realloced_calls->size * sizeof(*global_stats->realloced_calls->nodes));

    global_stats->realloced_calls->nodes[previous_size].ptr = ptr;
    global_stats->realloced_calls->nodes[previous_size].new_size = new_size;

    return realloc(ptr, new_size);
}


MockMallocStats* mock_malloc_create_stats(void) {
    if (global_stats != NULL) {
        fprintf(stderr, "Error, stats already created\n");
        exit(4);
    }

    global_stats = malloc(sizeof(MockMallocStats));

    global_stats->realloced_calls = malloc(sizeof(DynArrayR));
    global_stats->realloced_calls->size = 0;
    global_stats->realloced_calls->nodes = NULL;

    global_stats->free_calls = malloc(sizeof(DynArrayF));
    global_stats->free_calls->size = 0;
    global_stats->free_calls->nodes = NULL;

    global_stats->realloc_fail_count = 0;

    return global_stats;
}

void mock_malloc_free_stats(MockMallocStats* stats) {
    if (global_stats == NULL) {
        fprintf(stderr, "Error, stats already freed\n");
        exit(4);
    }

    if (global_stats != stats) {
        fprintf(stderr, "Error, stats niot the same as the global instance\n");
        exit(4);
    }


    free(global_stats->free_calls->nodes);
    free(global_stats->free_calls);

    free(global_stats->realloced_calls->nodes);
    free(global_stats->realloced_calls);

    free(global_stats);

    global_stats = NULL;
}


uint64_t mock_malloc_count_realloc(MockMallocStats* stats) {
    assert(global_stats);
    return stats->realloced_calls->size;
}


uint64_t mock_malloc_count_free(MockMallocStats* stats) {
    assert(global_stats);
    return stats->free_calls->size;
}

void mock_fail_next_realloc_call(void) {
    mock_fail_next_realloc_calls(1);
}


void mock_fail_next_realloc_calls(size_t size) {
    assert(global_stats);

    global_stats->realloc_fail_count += size;
}

size_t mock_next_realloc_fail_count(void) {
    return global_stats->realloc_fail_count;
}
