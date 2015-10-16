#ifndef CONSTRUCT_TOKEN_H_
#define CONSTRUCT_TOKEN_H_

#include "util/types.h"
#include "util/uthash.h"

typedef struct {
  // Key
  wchar_t* name;

  // Payload
  unsigned int* docIds;  // Posting list
  unsigned int length;  // Number of filled docIds
  unsigned int listSize;  // Number of allocated docIds

  // Macro used in order to store use this as a uthash
  UT_hash_handle hh;
} Token;

// Displays a token key and payload on the output
void fprintToken(FILE* output, const Token* t);

// Adds an occurence of the tokenName, docId pair in the vocabulary
Token* addToken(Token* vocabulary, wchar_t* tokenName, unsigned int docId);

// Frees the memory of a Token and all it points to.
// CAUTION: does NOT remove the token pointer from vocabulary, you MUST do it!
void deleteToken(Token* t);

#endif  // CONSTRUCT_TOKEN_H_
