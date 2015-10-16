#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "token.h"
#include "util/parser.h"
#include "util/uthash.h"

int main() {
  FILE* input = stdin;
  FILE* output = stdout;
  Token* vocabulary = NULL;
  wchar_t buffer[kBufferSize];

  for (unsigned int docId = 0; !feof(input); ++docId) {
    buffer[0] = L'\0';  // Set to empty string
    wchar_t* result = fgetws(buffer, kBufferSize, input);
    if (!result) {
      if (!feof(input)) return 1;
      else break;
    }

    bool endOfBuffer = false;
    do {
      wchar_t tokenName[kBufferSize];
      tokenName[0] = L'\0';  // Initialize to empty string
      if (!nextToken(buffer, input, tokenName, &endOfBuffer)) return 1;

      vocabulary = addToken(vocabulary, tokenName, docId);
      if (!vocabulary) return 1;
    } while (!endOfBuffer);
  }

  // Print and free the hash table contents
  Token* t = NULL;
  Token* tmp = NULL;
  HASH_ITER(hh, vocabulary, t, tmp) {
    fprintToken(output, t);

    HASH_DEL(vocabulary, t);
    deleteToken(t);
    t = NULL;
  }
  free(vocabulary);
  vocabulary = NULL;

  return 0;
}
