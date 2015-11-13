#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "util/index.h"
#include "util/parser.h"
#include "util/pmemory.h"
#include "util/types.h"

/*Returns an array of int containing the indexes of the entries to be elected
from termEntries, an writes the size of the array in outElectedEntriesSize*/
int* electedEntries (TermEntry* termEntries, int termEntriesSize,
    int* outElectedEntriesSize) {
  wchar_t* electedWord = NULL;
  int currentElectedEntriesSize = 0;
  int* electedEntries;

  for (int i = 0; i < termEntriesSize; i++) {
    if (electedWord == NULL ||
        wcscmp(electedWord, termEntries[i].token) > 0) {
      electedWord = termEntries[i].token;
      currentElectedEntriesSize = 1;
    } else if (wcscmp(electedWord, termEntries[i].token) == 0) {
      currentElectedEntriesSize++;
    }
  }

  *outElectedEntriesSize = currentElectedEntriesSize;
  electedEntries = (int*) pMalloc(currentElectedEntriesSize * sizeof(int));
  int currentElectedIndex = 0;
  for (int i = 0; i < termEntriesSize; i++) {
    if (wcscmp(electedWord, termEntries[i].token) == 0) {
      electedEntries[currentElectedIndex++] = i;
    }
  }

  return electedEntries;
}

int main() {
  setlocale(LC_ALL, "en_US.UTF-8");  // For unicode handling

  FILE* input = stdin;

  unsigned int currentSize = kInitialNbFiles;
  unsigned int currentLength = 0;
  FILE** openFiles = (FILE**) pMalloc(sizeof(FILE*) * currentSize);
  if (!openFiles) return MEMORY_ERROR;

  // Open an array of files
  for (unsigned int i = 0; !feof(input); ++i) {
    char filepath[kBufferSize];
    memset(filepath, 0, kBufferSize);

    if (!fgets(filepath, kBufferSize, input)) {
      if (!feof(input)) {
        return INPUT_ERROR;
      } else {
        break;
      }
    }

    unsigned int sizePath = strlen(filepath);
    if (filepath[sizePath - 1] == '\n') filepath[sizePath - 1] = '\0';

    if (i >= currentSize) {
      unsigned int previousSize = currentSize * sizeof(FILE*);
      currentSize *= 2;
      openFiles = (FILE**) pRealloc(openFiles, previousSize,
                                    currentSize * sizeof(FILE*));
      if (!openFiles) return MEMORY_ERROR;
    }

    openFiles[i] = fopen(filepath, "r");
    ++currentLength;
  }

  // Close all files and free array
  for (unsigned int i = 0; i < currentLength; ++i) fclose(openFiles[i]);
  pFree(openFiles, currentSize * sizeof(FILE*));

  return 0;
}
