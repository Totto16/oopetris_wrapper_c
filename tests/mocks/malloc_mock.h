

#pragma once

#include <stddef.h>
#include <stdint.h>


void mock_free(void* ptr);

void* mock_realloc(void* ptr, size_t new_size);

// opaque types

struct MockMallocStatsImpl;

typedef struct MockMallocStatsImpl MockMallocStats;

MockMallocStats* mock_malloc_create_stats(void);

void mock_malloc_free_stats(MockMallocStats* stats);

uint64_t mock_malloc_count_realloc(MockMallocStats* stats);

uint64_t mock_malloc_count_free(MockMallocStats* stats);

void mock_fail_next_realloc_call(void);

void mock_fail_next_realloc_calls(size_t size);

size_t mock_next_realloc_fail_count(void);
