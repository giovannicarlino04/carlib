#include "common.h"

// Simple custom memory pool (we'll allocate a large chunk of memory at once)
#define GC_PAGE_SIZE 4096  // Define page size as 4KB (typical page size)
#define MEMORY_POOL_SIZE (GC_PAGE_SIZE * 10)  // 10 pages for our memory pool

// Memory pool: a static block of memory we manage
static BYTE memory_pool[MEMORY_POOL_SIZE];

// Pointer to the current "free" position in the memory pool
static BYTE* memory_ptr = memory_pool;

// Free list: stores pointers to free memory blocks
typedef struct Block {
    size_t size;  // Size of the block
    struct Block* next;  // Pointer to the next free block
} Block;

static Block* free_list = NULL;  // Head of the free list

#define MAX_SIZE 2048

// Function to initialize the memory pool (called once during initialization)
internal void init_memory_pool() {
    memory_ptr = memory_pool;  // Reset the pointer to the beginning of the memory pool
    free_list = NULL;  // Start with no free blocks
}

size_t align(size_t size) {
    // Align size to the nearest multiple of 8 bytes
    return (size + 7) & ~7;
 }  

// Custom malloc (gc_malloc) without using stdlib's malloc
DLLEXPORT void* gc_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    // Align the memory to ensure it meets the required alignment
    size = align(size);

    // First, check the free list for available blocks
    Block* prev = NULL;
    Block* current = free_list;

    while (current) {
        if (current->size >= size) {
            // Found a suitable free block
            if (current->size > size + sizeof(Block)) {
                // If the block is larger than needed, split it
                Block* new_block = (Block*)((BYTE*)current + sizeof(Block) + size);
                new_block->size = current->size - size - sizeof(Block);
                new_block->next = current->next;

                // Update the current block size
                current->size = size;
                current->next = NULL;
            }

            // Remove the block from the free list
            if (prev) {
                prev->next = current->next;
            } else {
                free_list = current->next;
            }

            // Return the memory just after the Block header
            return (BYTE*)current + sizeof(Block);
        }

        prev = current;
        current = current->next;
    }

    // No suitable free block found, allocate new memory from the pool
    if (memory_ptr + size + sizeof(Block) > memory_pool + MEMORY_POOL_SIZE) {
        // Out of memory in the pool
        return NULL;
    }

    // Allocate memory from the pool
    void* allocated_memory = memory_ptr;
    memory_ptr += size + sizeof(Block);

    // Initialize the Block header
    Block* new_block = (Block*)allocated_memory;
    new_block->size = size;
    new_block->next = NULL;

    return (BYTE*)new_block + sizeof(Block);
}

// Custom free (gc_free) without using stdlib's free
DLLEXPORT void gc_free(void* ptr) {
    if (!ptr) {
        return;  // No need to free NULL pointer
    }

    // Get the block header, which is just before the pointer
    Block* block_to_free = (Block*)((BYTE*)ptr - sizeof(Block));

    // Add the block back to the free list
    block_to_free->next = free_list;
    free_list = block_to_free;

    // Optional: Merge adjacent free blocks (coalescing)
    Block* current = free_list;
    while (current && current->next) {
        // Check if the current block and next block are adjacent
        if ((BYTE*)current + current->size + sizeof(Block) == (BYTE*)current->next) {
            // Coalesce adjacent blocks
            current->size += current->next->size + sizeof(Block);
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

