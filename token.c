#include <wchar.h>

#include "token.h"

#include "util/uthash.h"

// Initial size of the posting list array
static const unsigned int kInitialPostingListSize = 5;

// By how much the size is multiplied when reallocating posting list
static const unsigned int kSizeAugmentFactor = 2;

// See token.h
void fprintToken(FILE* output, const Token* t) {
  fprintf(output, "%ls: %u", t->name, t->docIds[0]);
  for (unsigned int i = 1; i < t->length; ++i) {
    fprintf(output, ", %u", t->docIds[i]);
  }
  fprintf(output, "\n");
}

// See token.h
Token* addToken(Token* vocabulary, wchar_t* tokenName, unsigned int docId) {
  Token* token = NULL;

  HASH_FIND(hh, vocabulary, tokenName,
        wcslen(tokenName) * sizeof(wchar_t), token);
  if (!token) {
    // Not found: create Token structure, populate it and add it to vocabulary
    token = (Token*) malloc(sizeof(Token));
    if (!token) return NULL;

    token->name = (wchar_t*) malloc(sizeof(wchar_t) * (wcslen(tokenName) + 1));
    if (!token->name) return NULL;
    token->name = wcscpy(token->name, tokenName);

    token->docIds = (unsigned int*) malloc(sizeof(unsigned int)
                    * kInitialPostingListSize);
    if (!token->docIds) return NULL;

    token->length = 1;
    token->listSize = kInitialPostingListSize;
    token->docIds[0] = docId;

    // Add it to the hash map
    HASH_ADD_KEYPTR(hh, vocabulary, token->name,
            wcslen(token->name) * sizeof(wchar_t), token);
  } else if (docId != token->docIds[token->length - 1]) {
    // Found and not duplicate: just adding the docId to the list
    if (token->length >= token->listSize) {
      token->listSize *= kSizeAugmentFactor;
      token->docIds = (unsigned int*) realloc(token->docIds, token->listSize);
      if (!token->docIds) return NULL;
    }
    token->docIds[token->length++] = docId;  // Adding the docId to the list
  }

  return vocabulary;
}

// See token.h
void deleteToken(Token* t) {
  free(t->docIds);
  t->docIds = NULL;
  free(t->name);
  t->name = NULL;
  free(t);
  t = NULL;
}
