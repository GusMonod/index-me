#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "construct/index.h"
#include "util/parser.h"
#include "util/uthash.h"

const char outputDirectory[] = "./";
const unsigned int kMaxFilenameSize = 20;

// Creates a file and serializes the index to it before freeing all its memory.
static Vocabulary* purgeIndex(Vocabulary* vocabulary);

int main(int argc, char** argv) {
  setlocale(LC_ALL, "en_US.UTF-8");  // For unicode handling

  // Parsing command line arguments
  TermPrintMode requestedPrintMode;
  if (argc > 1) {
    if((strcmp(argv[1], "-tfidf") == 0))
      requestedPrintMode = TEST_TFIDF;
    else if ((strcmp(argv[1], "-serialization") == 0))
      requestedPrintMode = SERIALIZATION;
    else
      requestedPrintMode = TEST_SIMPLE;
  } else{
    requestedPrintMode = TEST_SIMPLE;
  }

  FILE* input = stdin;
  FILE* output = stdout;

  Vocabulary* vocabulary = NULL;

  wchar_t* buffer = (wchar_t*) malloc(sizeof(wchar_t) * kBufferSize);
  for (unsigned int docId = 0; !feof(input); ++docId) {
    buffer[0] = L'\0';  // Set to empty string
    buffer = fgetws(buffer, kBufferSize, input);

    if (!buffer) {
      if (!feof(input)) {
        return 1;
      } else {
        break;
      }
    }

    while (!endOfBuffer(buffer)) {
      wchar_t token[kBufferSize];
      token[0] = L'\0';  // Initialize to empty string
      if (!nextToken(buffer, input, token)) return 1;

      bool noMemory = false;
      vocabulary = tryToAddToken(vocabulary, token, docId, &noMemory);
      if (noMemory) {
        vocabulary = purgeIndex(vocabulary);
        if (vocabulary) return 1;  // Vocabulary not empty: could not purge

        // Adding again (once purged)
        vocabulary = tryToAddToken(vocabulary, token, docId, &noMemory);
        if (noMemory) return 1;  // Could not add
      }
      if (!vocabulary) return 1;  // Vocabulary is still empty
    }
  }
  free(buffer);
  buffer = NULL;

  if (requestedPrintMode == SERIALIZATION) {
    HASH_SORT(vocabulary, compareTermEntries);
  }

  // Print and free the hash table contents
  TermEntry* t = NULL;
  TermEntry* tmp = NULL;
  HASH_ITER(hh, vocabulary, t, tmp) {
    fprintTerm(output, t, requestedPrintMode);
  }

  vocabulary = purgeIndex(vocabulary);
  if (vocabulary) return 1;  // Vocabulary not empty: could not purge

  return 0;
}

// See top of this file
static Vocabulary* purgeIndex(Vocabulary* vocabulary) {
  static unsigned int nbIndexParts = 0;

  unsigned int maxSize = strlen(outputDirectory) + kMaxFilenameSize + 1;
  char* filepath = (char*) malloc(maxSize);
  snprintf(filepath, maxSize, "%s/index-%u.part",
           outputDirectory, nbIndexParts);
  FILE* save = fopen(filepath, "w+");
  vocabulary = fpurgeIndex(save, vocabulary);
  fclose(save);
  free(filepath);
  filepath = NULL;

  ++nbIndexParts;

  return vocabulary;
}
