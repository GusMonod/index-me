#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "util/index.h"
#include "util/parser.h"
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

  while (!feof(input)) {
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

    FILE* index = fopen(filepath, "r");

    printf("%s:\n\n", filepath);
    while (!feof(index)) {
      TermEntry* term = readTermEntry(index);
      if (!term) return PARSE_ERROR;

      fprintTerm(stdout, term, TEST_SIMPLE);
    }

    fclose(index);
    index = NULL;
  }

  return 0;
}
