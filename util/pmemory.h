#ifndef UTIL_PMEMORY_H_
#define UTIL_PMEMORY_H_

#include <stdint.h>
#include <stdlib.h>

unsigned int gAllocatedBytes;

// These functions behave as malloc and free, except that the max size of the
// available memory is set in a constant in util/pmemory.c, making it possible
// to artificially reduce the available memory.

// Allocates memory of the given size and returns a pointer to the first byte.
// Returns NULL if there is no sufficient memory left.
void* pMalloc(size_t nbBytes);

// Tries to change the size of the allocation pointed to by ptr from oldSize
// to newSize (in bytes). Note that the size cannot be made smaller.
// Returns NULL if there is no sufficient memory left.
void* pRealloc(void* ptr, size_t oldSize, size_t newSize);

// Frees all the memory allocated by pmalloc.
void pFree(void* ptr, size_t size);

#endif  // UTIL_PMEMORY_H_
