

#pragma once

#include <stddef.h>
#include <stdint.h>

// opaque type

void mock_free(void* ptr);

void* mock_realloc(void* ptr, size_t new_size);

struct MockMallocStatsImpl;

typedef struct MockMallocStatsImpl MockMallocStats;

MockMallocStats* mock_malloc_create_stats(void);

void mock_malloc_free_stats(MockMallocStats* stats);

uint64_t mock_malloc_count_realloc(MockMallocStats* stats);

uint64_t mock_malloc_count_free(MockMallocStats* stats);
