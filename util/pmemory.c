#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/pmemory.h"

static const size_t kMemorySizeBytes = 8 << 20;  // 8 MB

// Number of allocated bytes
static unsigned int gAllocatedBytes;

// See util/pmemory.h
void* pMalloc(size_t nbBytes) {
  // Return NULL if not enough space left
  if (gAllocatedBytes + nbBytes > kMemorySizeBytes) return NULL;

  gAllocatedBytes += nbBytes;
  return malloc(nbBytes);
}

// See util/pmemory.h
void* pRealloc(void* initialPtr, size_t oldSize, size_t newSize) {
  int nbBytes = newSize - oldSize;
  if (nbBytes <= 0) return NULL;

  // Return NULL if not enough space left
  if (gAllocatedBytes + nbBytes > kMemorySizeBytes) return NULL;

  gAllocatedBytes += nbBytes;
  return realloc(initialPtr, newSize);
}

// See util/pmemory.h
void pFree(void* ptr, size_t size) {
  gAllocatedBytes -= size;
  free(ptr);
}
