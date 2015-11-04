#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/pmemory.h"

static const size_t kMemorySizeBytes = 8 << 20;

// Points to the beginning of the big allocated chunk
static uint8_t* gBasePointer = NULL;

// Points to the last used byte (by the last call to pmalloc)
static uint8_t* gEndPointer = NULL;

// See util/pmemory.h
void* pMalloc(size_t nbBytes) {
  // Allocates the big chunk if it isn't already
  if (!gBasePointer) gEndPointer = gBasePointer = malloc(kMemorySizeBytes);

  // If the nbBytes left after gEndPointer (included) is too low => no memory
  if (gEndPointer + nbBytes > gBasePointer + kMemorySizeBytes) return NULL;

  // Will return the first available byte before this call was made
  void* result = (void*) gEndPointer;

  // Now points to the next available byte (or after the last available byte)
  gEndPointer += nbBytes;

  return result;
}

// See util/pmemory.h
void* pRealloc(void* initialPtr, size_t oldSize, size_t newSize) {
  // TODO fix this function, currently buggy

  if (newSize < oldSize) return initialPtr;  // Cannot make the size smaller

  // Allocates the big chunk if it isn't already
  if (!gBasePointer) gEndPointer = gBasePointer = malloc(kMemorySizeBytes);

  unsigned int nbBytes = newSize - oldSize;
  // If the nbBytes left after gEndPointer (included) is too low => no memory
  if (gEndPointer + nbBytes > gBasePointer + kMemorySizeBytes) return NULL;

  // Moving all of the memory after (initialPtr + oldSize) nbBytes to the right
  uint8_t* source = gEndPointer - 1;
  uint8_t* destination = source + nbBytes;
  uint8_t* sourceEnd = ((uint8_t*) initialPtr) + oldSize;
  while (source >= sourceEnd) {
    *destination = *source;
    --destination;
    --source;
  }

  return initialPtr;
}

// See util/pmemory.h
void pFreeAll() {
  free(gBasePointer);
  gBasePointer = NULL;
}

/*
void print(uint8_t* complete) {
  printf("[%2d", complete[0]);
  for (int i = 1; i < kMemorySizeBytes; ++i) {
    printf(", %2d", complete[i]);
  }
  printf("]\n");
}

int main() {
  uint8_t* pA = pMalloc(3);
  uint8_t* complete = pA;
  memset(complete, 0, kMemorySizeBytes);
  pA[0] = 0;
  pA[1] = 1;
  pA[2] = 2;

  print(complete);

  uint8_t* pB = pMalloc(3);
  pB[0] = 10;
  pB[1] = 11;
  pB[2] = 12;

  print(complete);

  pRealloc(pA, 3, 7);

  print(complete);

  pA[3] = 3;
  pA[4] = 4;

  print(complete);

}
*/
