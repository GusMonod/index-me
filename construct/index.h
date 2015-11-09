#ifndef CONSTRUCT_TOKEN_H_
#define CONSTRUCT_TOKEN_H_

#include <stdio.h>

#include "util/types.h"
#include "util/uthash.h"

// Posting list entry used in an array of PostingListEntries
typedef struct {
  unsigned int docId;
  unsigned int occurrences;  // Occurrences of the term in the document
} PostingListEntry;

// Term entry used in a hash table pointing to a PostingListEntry array
typedef struct {
  // Key: the token name
  wchar_t* token;

  // Payload: PostingListEntry array
  PostingListEntry* postingList;  // Posting list table
  unsigned int listLength;  // Number of filled PostingListEntry
  unsigned int listSize;  // Number of allocated PostingListEntry

  // Macro used in order to use this as a uthash hash table
  UT_hash_handle hh;
} TermEntry;

// How to print the term
typedef enum {
  TEST_SIMPLE,
  TEST_TFIDF,
  SERIALIZATION
} TermPrintMode;

// A TermEntry is a hash table itself (see uthash doc)
typedef TermEntry Vocabulary;

// Adds token occurrence for a docId in the index (vocab + posting list).
// Assumes docId is equal to or one more than the docId of last call.
// Sets noMemory to true if it was impossible to add the token, false otherwise.
// Returns the vocabulary once the token was added.
Vocabulary* tryToAddToken(Vocabulary* vocabulary, wchar_t* token,
                          unsigned int docId, bool* noMemory);

// Serializes the index (vocab + posting list) to output and frees its memory.
// The output file should be open in text and write mode.
// Returns the vocabulary once the index is purged.
Vocabulary* fpurgeIndex(FILE* output, Vocabulary* vocabulary);

// Displays a token key and payload on the output.
// If printMode is set to TF_IDF, the frequencies are also displayed.
// If it is set to SERIALIZATION, the output is printed like is is meant to be
// serialized.
void fprintTerm(FILE* output, const TermEntry* t, TermPrintMode printMode);

// Frees the memory of a TermEntry and all it points to, including posting list.
// CAUTION: does NOT remove the token pointer from Vocabulary, you MUST do it!
void freeTerm(TermEntry* t);

#endif  // CONSTRUCT_TOKEN_H_
