#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "construct/token.h"
#include "util/parser.h"
#include "util/uthash.h"

int main(int argc, char** argv) {
  setlocale(LC_ALL, "");  // For unicode handling

  bool printFrequencies = (argc > 1 && (strcmp(argv[1], "-tfidf") == 0));

  FILE* input = stdin;
  FILE* output = stdout;
  Token* vocabulary = NULL;

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
      wchar_t tokenName[kBufferSize];
      tokenName[0] = L'\0';  // Initialize to empty string
      if (!nextToken(buffer, input, tokenName)) return 1;

      vocabulary = addToken(vocabulary, tokenName, docId);
      if (!vocabulary) return 1;
    }
  }
  free(buffer);
  buffer = NULL;

  // Print and free the hash table contents
  Token* t = NULL;
  Token* tmp = NULL;
  HASH_ITER(hh, vocabulary, t, tmp) {
    fprintToken(output, t, printFrequencies);
  }
  vocabulary = saveAndCleanVocabulary(vocabulary);
  free(vocabulary);
  vocabulary = NULL;

  return 0;
}
