#ifndef MEMORY_H
#define MEMORY_H

#include "gc_debug.h"
#include "gc_defs.h"

// Simple custom memory pool (we'll allocate a large chunk of memory at once)
#define MEMORY_POOL_SIZE (GC_PAGE_SIZE * 10)  // 10 pages for our memory pool

// Memory pool: a static block of memory we manage
static gc_byte memory_pool[MEMORY_POOL_SIZE];

// Pointer to the current "free" position in the memory pool
static gc_byte* memory_ptr = memory_pool;

// Free list: stores pointers to free memory blocks
typedef struct Block {
    size_t size;  // Size of the block
    struct Block* next;  // Pointer to the next free block
} Block;

static Block* free_list = NULL;  // Head of the free list

#define MAX_SIZE 2048

// Custom malloc and free functions
void* gc_malloc(gc_size size);
void gc_free(void* ptr);

#endif // MEMORY_H
