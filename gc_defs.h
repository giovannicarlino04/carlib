#ifndef GC_DEFS_H
#define GC_DEFS_H

// Fundamental type definitions
typedef unsigned char gc_byte;   // For byte-level operations
typedef unsigned int gc_uint;    // For general unsigned integers
typedef unsigned long gc_size;   // For size values (platform-dependent)

// Basic constants
#define GC_ALIGNMENT 8           // Memory alignment (8 bytes for 64-bit systems)
#define GC_PAGE_SIZE 4096        // Standard page size for memory chunks
#define GC_HEADER_SIZE 16        // Metadata size for custom allocations

// Debugging macros
#define GC_DEBUG 1               // Enable or disable debug logging
#if GC_DEBUG
    #define GC_LOG(...) printf(__VA_ARGS__) // Debug log
#else
    #define GC_LOG(...)          // No-op if GC_DEBUG is disabled
#endif

// Error codes
#define GC_SUCCESS 0             // Successful operation
#define GC_ERR_OUT_OF_MEMORY -1  // Allocation failure
#define GC_ERR_INVALID_FREE -2   // Attempt to free an invalid pointer

// Utility macros
#define GC_ALIGN(size) (((size) + (GC_ALIGNMENT - 1)) & ~(GC_ALIGNMENT - 1)) // Align size to GC_ALIGNMENT
#define GC_MIN(a, b) ((a) < (b) ? (a) : (b))                                // Minimum of two values
#define GC_MAX(a, b) ((a) > (b) ? (a) : (b))                                // Maximum of two values
#define GC_HEAP GetProcessHeap()

#define uchar unsigned char
#define int8_t signed char
#define int16_t signed short
#define int32_t signed int
#define int64_t signed long long
#define uint64_t unsigned long long
#define uint32_t unsigned int
#define uint16_t unsigned short
#define uint8_t unsigned char
#define uint16 uint16_t
#define uint32 uint32_t
#define uint64 uint64_t
typedef unsigned int size_t;

#endif // GC_DEFS_H
