#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "construct/index.h"
#include "util/parser.h"
#include "util/pmemory.h"
#include "util/types.h"
#include "util/uthash.h"

const char outputDirectory[] = ".";
const unsigned int kMaxFilenameSize = 20;

// Frees the index and serializes it to a file (unless print is set to false).
// Also prints the filename to output.
static Vocabulary* purgeIndex(FILE* output, Vocabulary* vocabulary, bool print);

int main(int argc, char** argv) {
  setlocale(LC_ALL, "en_US.UTF-8");  // For unicode handling

  bool print = false;
  // Parsing command line arguments
  TermPrintMode requestedPrintMode;

  if (argc > 1 && (strcmp(argv[1], "-tfidf") == 0)) {
    requestedPrintMode = TEST_TFIDF;
  } else if (argc > 1 && (strcmp(argv[1], "-simple") == 0)) {
    requestedPrintMode = TEST_SIMPLE;
  } else {
    print = true;
    requestedPrintMode = SERIALIZATION;
  }

  FILE* input = stdin;
  FILE* output = stdout;

  Vocabulary* vocabulary = NULL;

  for (unsigned int docId = 0; !feof(input); ++docId) {
    wchar_t buffer[kBufferSize];
    memset(buffer, 0, kBufferSize * sizeof(wchar_t));  // Set to empty string

    if (!fgetws(buffer, kBufferSize, input)) {
      if (!feof(input)) {
        return INPUT_ERROR;
      } else {
        break;
      }
    }

    while (!endOfBuffer(buffer)) {
      wchar_t token[kBufferSize];
      memset(token, 0, kBufferSize * sizeof(wchar_t));  // Set to empty string
      if (!nextToken(buffer, input, token)) {
        return PARSE_ERROR;
      }

      bool noMemory = false;
      vocabulary = tryToAddToken(vocabulary, token, docId, &noMemory);
      if (noMemory) {
        vocabulary = purgeIndex(output, vocabulary, print);
        if (vocabulary) return PURGE_ERROR;  // Vocabulary not empty

        // Adding again (once purged)
        vocabulary = tryToAddToken(vocabulary, token, docId, &noMemory);
        if (noMemory) return ADD_ERROR;  // Could not add
      }
      if (!vocabulary) return ADD_ERROR;  // Vocabulary is still empty
    }
  }

  if (requestedPrintMode != SERIALIZATION) {
    // Print the hash table contents
    TermEntry* t = NULL;
    TermEntry* tmp = NULL;
    HASH_ITER(hh, vocabulary, t, tmp) {
      fprintTerm(output, t, requestedPrintMode);
    }
  } else {
    HASH_SORT(vocabulary, compareTermEntries);
  }

  vocabulary = purgeIndex(output, vocabulary, print);
  if (vocabulary) return PURGE_ERROR;  // Vocabulary not empty

  return 0;
}

// See top of this file
static Vocabulary* purgeIndex(FILE* output, Vocabulary* vocabulary,
                              bool print) {
  static unsigned int nbIndexParts = 0;

  // Generate filepath
  unsigned int maxSize = strlen(outputDirectory) + kMaxFilenameSize + 1;
  char filepath[maxSize];
  if (!print) {
    strcpy(filepath, "/dev/null");  // No file is created
  } else {
    snprintf(filepath, maxSize, "%s/index-%u.part",
             outputDirectory, nbIndexParts);
  }

  // Write to file
  FILE* save = fopen(filepath, "w+");
  vocabulary = fpurgeIndex(save, vocabulary);
  fclose(save);

  // Print and free filepath
  if (print) {
    fprintf(output, "%s\n", filepath);
  }

  ++nbIndexParts;

  return vocabulary;
}
