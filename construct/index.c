#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "construct/index.h"

#include "util/pmemory.h"
#include "util/uthash.h"

// Number of documents handled
static unsigned int documentsCountInCorpus = 0;

// Initial size of the posting list array
static const unsigned int kInitialPostingListSize = 10;

// By how much the size is multiplied when reallocating posting list
static const unsigned int kSizeAugmentFactor = 2;

// Returns the idf (inverse document frequency) of a term
static double inverseDocumentFrequency(const TermEntry* term);

// Returns the number of occurrences of a token in a document
static unsigned int termFrequency(const TermEntry* term,
                                  unsigned int documentId);

// Adds one to the occurrence count of a term for a docId.
// May resize postingList if the docId is not already present.
// Returns true if there is no memory left, false otherwise.
static bool addOneToTermEntry(TermEntry* term, unsigned int docId);

// Adds a new TermEntry (must alredy be allocated) to the vocabulary.
static Vocabulary* addNewTermEntry(Vocabulary* vocabulary, TermEntry* term,
                                   wchar_t* token, unsigned int docId);

//See construct/index.h
int compareTermEntries(TermEntry* t1, TermEntry* t2){
  return wcscmp(t1->token, t2->token);
}

// See construct/token.h
Vocabulary* tryToAddToken(Vocabulary* vocabulary, wchar_t* token,
                          unsigned int docId, bool* noMemory) {
  // Add one to count if this is a new document
  if (docId + 1 > documentsCountInCorpus) ++documentsCountInCorpus;

  TermEntry* term = NULL;

  HASH_FIND(hh, vocabulary, token,
            wcslen(token) * sizeof(wchar_t), term);
  if (!term) {
    // Not found: allocate TermEntry structure
    term = (TermEntry*) pMalloc(sizeof(TermEntry));
    if (!term) {
      /*
      free(term);
      term = NULL;
      */
      *noMemory = true;
      return vocabulary;
    }

    term->token = (wchar_t*) pMalloc(sizeof(wchar_t) * (wcslen(token) + 1));
    if (!term->token) {
      /*
      free(term->token);
      term->token = NULL;

      free(term);
      term = NULL;
      */
      *noMemory = true;
      return vocabulary;
    }

    term->postingList = (PostingListEntry*) pMalloc(sizeof(PostingListEntry)
                                                    * kInitialPostingListSize);
    if (!term->postingList) {
      /*
      free(term->postingList);
      term->postingList = NULL;

      free(term->token);
      term->token = NULL;

      free(term);
      term = NULL;
      */
      *noMemory = true;
      return vocabulary;
    }

    // Populate term and add it to vocab
    vocabulary = addNewTermEntry(vocabulary, term, token, docId);
  } else {
    // Found and not duplicate: just adding the docId to the list
    *noMemory = addOneToTermEntry(term, docId);
  }

  return vocabulary;
}

// See construct/token.h
Vocabulary* fpurgeIndex(FILE* output, Vocabulary* vocabulary) {
  //Sort the hash table to serialize a sorted index
  HASH_SORT(vocabulary, compareTermEntries);

  TermEntry* t = NULL;
  TermEntry* tmp = NULL;
  HASH_ITER(hh, vocabulary, t, tmp) {
    fprintTerm(output, t, SERIALIZATION);
    HASH_DEL(vocabulary, t);
    pFreeTerm(t);
    t = NULL;
  }

  return vocabulary;
}

// See construct/token.h
void fprintTerm(FILE* output, const TermEntry* t, TermPrintMode printMode) {
  switch (printMode) {
    case TEST_TFIDF:
      fprintf(output, "%ls: ", t->token);
      fprintf(output, "idf=%.2f, in docs: %u(tf = %d)",
              inverseDocumentFrequency(t),
              t->postingList[0].docId,
              termFrequency(t, t->postingList[0].docId));
      for (unsigned int i = 1; i < t->listLength; ++i) {
        fprintf(output, ", %u(tf = %d)",
                t->postingList[i].docId,
                termFrequency(t, t->postingList[i].docId));
      }
      fprintf(output, "\n");
      break;

    case TEST_SIMPLE:
      fprintf(output, "%ls: ", t->token);
      fprintf(output, "%u", t->postingList[0].docId);
      for (unsigned int i = 1; i < t->listLength; ++i) {
        fprintf(output, ", %u", t->postingList[i].docId);
      }
      fprintf(output, "\n");
      break;

    case SERIALIZATION:
      fprintf(output, "%ls\n", t->token);
      fprintf(output, "%u %u", t->postingList[0].docId,
              t->postingList[0].occurrences);
      for (unsigned int i = 1; i < t->listLength; ++i) {
        fprintf(output, " %u %u", t->postingList[i].docId,
                t->postingList[i].occurrences);
      }
      fprintf(output, "\n");
      break;
  }
}

// See construct/token.h
void pFreeTerm(TermEntry* t) {
  pFree(t->postingList, t->listSize * sizeof(PostingListEntry));
  t->postingList = NULL;
  pFree(t->token, wcslen(t->token) * sizeof(wchar_t));
  t->token = NULL;
  pFree(t, sizeof(TermEntry));
  t = NULL;
}


// See top of this file
static double inverseDocumentFrequency(const TermEntry* term) {
  return log10((double) documentsCountInCorpus / term->listLength);
}

// See top of this file
static unsigned int termFrequency(const TermEntry* term,
                                  unsigned int documentId) {
  for (unsigned int i = 0; i < term->listLength; ++i) {
    if (term->postingList[i].docId == documentId) {
      return term->postingList[i].occurrences;
    }
  }
  return 0;  // Not in the document
}

// See top of this file
static bool addOneToTermEntry(TermEntry* term, unsigned int docId) {
  if (term->listLength >= term->listSize) {
    // TODO fix pRealloc and remove the following line
    return true;  // pRealloc is buggy: stop now

    unsigned int previousSize = term->listSize * sizeof(PostingListEntry);
    term->listSize *= kSizeAugmentFactor;
    term->postingList = (PostingListEntry*)
                         pRealloc(term->postingList, previousSize,
                                  term->listSize * sizeof(PostingListEntry));
    if (!term->postingList) return true;  // No memory left
  }

  unsigned int docIdFound;
  // If the document id is in the posting list, just increment occurrences
  for (docIdFound = 0; docIdFound < term->listLength; ++docIdFound) {
    if (term->postingList[docIdFound].docId == docId){
      term->postingList[docIdFound].occurrences++;
      break;
    }
  }

  // If no entry for this term in this document yet, add docId to the list
  if (docIdFound == term->listLength) {
    term->listLength++;
    term->postingList[term->listLength - 1].docId = docId;
    term->postingList[term->listLength - 1].occurrences = 1;
  }

  return false;
}

// See top of this file
static Vocabulary* addNewTermEntry(Vocabulary* vocabulary, TermEntry* term,
                                   wchar_t* token, unsigned int docId) {
  wcscpy(term->token, token);

  term->listLength = 1;
  term->listSize = kInitialPostingListSize;
  term->postingList[0].docId = docId;
  term->postingList[0].occurrences = 1;

  // Add it to the hash map
  HASH_ADD_KEYPTR(hh, vocabulary, term->token,
                  wcslen(term->token) * sizeof(wchar_t), term);

  return vocabulary;
}
