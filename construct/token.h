#ifndef CONSTRUCT_TOKEN_H_
#define CONSTRUCT_TOKEN_H_

#include <stdio.h>

#include "util/types.h"
#include "util/uthash.h"

typedef struct {
  unsigned int docId;
  unsigned int occurrences;  //Number of occurrences of the term in the document
} PostingListEntry;

typedef struct {
  // Key
  wchar_t* name;

  // Payload
  PostingListEntry* postingList;  // Posting list table
  unsigned int listLength;  // Number of filled PostingListEntry
  unsigned int listSize;  // Number of allocated PostingListEntry

  // Macro used in order to store use this as a uthash
  UT_hash_handle hh;
} Token;

// Displays a token key and payload on the output.
// If printFrequencies is set to true, the frequencies are also displayed.
void fprintToken(FILE* output, const Token* t, bool printFrequencies);

// Adds an occurence of the tokenName, docId pair in the vocabulary.
// Assumes docId is equal or one more than docId in last call
Token* addToken(Token* vocabulary, wchar_t* tokenName, unsigned int docId);

// Frees the memory of a Token and all it points to.
// CAUTION: does NOT remove the token pointer from vocabulary, you MUST do it!
void freeToken(Token* t);

#endif  // CONSTRUCT_TOKEN_H_
