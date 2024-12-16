#include "gc_memory.h"

// Function to initialize the memory pool (called once during initialization)
void init_memory_pool() {
    memory_ptr = memory_pool;  // Reset the pointer to the beginning of the memory pool
    free_list = NULL;  // Start with no free blocks
}

// Custom malloc (gc_malloc) without using stdlib's malloc
void* gc_malloc(gc_size size) {
    if (size == 0) {
        return NULL;
    }

    // Align the memory to ensure it meets the required alignment
    size = GC_ALIGN(size);

    // First, check the free list for available blocks
    Block* prev = NULL;
    Block* current = free_list;

    while (current) {
        if (current->size >= size) {
            // Found a suitable free block
            if (current->size > size + sizeof(Block)) {
                // If the block is larger than needed, split it
                Block* new_block = (Block*)((gc_byte*)current + sizeof(Block) + size);
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

            return (gc_byte*)current + sizeof(Block);  // Return the memory just after the Block header
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

    return (gc_byte*)new_block + sizeof(Block);
}

// Custom free (gc_free) without using stdlib's free
void gc_free(void* ptr) {
    if (!ptr) {
        return;  // No need to free NULL pointer
    }

    // Get the block header, which is just before the pointer
    Block* block_to_free = (Block*)((gc_byte*)ptr - sizeof(Block));

    // Add the block back to the free list
    block_to_free->next = free_list;
    free_list = block_to_free;

    // Optional: Merge adjacent free blocks (coalescing)
    Block* current = free_list;
    while (current && current->next) {
        if ((gc_byte*)current + current->size + sizeof(Block) == (gc_byte*)current->next) {
            // Coalesce adjacent blocks
            current->size += current->next->size + sizeof(Block);
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}
