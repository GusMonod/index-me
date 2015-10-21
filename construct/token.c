#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "token.h"

#include "util/uthash.h"

// Number of documents handled
static unsigned int documentsCountInCorpus = 0;

// Array associating each document id with its word count
static unsigned int* wordCountByDocumentId;

// Initial size of the posting list array
static const unsigned int kInitialPostingListSize = 5;

// By how much the size is multiplied when reallocating posting list
static const unsigned int kSizeAugmentFactor = 2;

static int compareTokens(Token* t1, Token* t2){
  return wcscmp(t1->name, t2->name);
}

// Returns the idf (inverse document frequency) of a Token
static double inverseDocumentFrequency(const Token* term) {
  return log10((double) documentsCountInCorpus / term->listLength);
}

// Returns the tf (term frequency) in a document of a Token
static double termFrequency(const Token* term, unsigned int documentId) {
  double occurrencesInDocument = 0;
  unsigned int i;
  for (i = 0; i < term->listLength; ++i) {
    if (term->postingList[i].docId == documentId) {
      occurrencesInDocument = (double) term->postingList[i].occurrences;
    }
  }

  if (documentId < documentsCountInCorpus) {
    return occurrencesInDocument/(double) wordCountByDocumentId[documentId];
  } else {
    return 0;
  }
}

// Adds one to the occurence number in the wordCountByDocumentId table.
// Returns true if there was no error, false otherwise.
static bool addOccurrence(unsigned int docId) {
  if (docId + 1 > documentsCountInCorpus) {
    documentsCountInCorpus = docId + 1;
    wordCountByDocumentId = (unsigned int*) realloc(wordCountByDocumentId,
                                                    documentsCountInCorpus
                                                    * sizeof(unsigned int));
    if (!wordCountByDocumentId) return false;
    wordCountByDocumentId[docId] = 0;
  }
  wordCountByDocumentId[docId]++;
  return true;
}

// See token.h
void fprintToken(FILE* output, const Token* t, TokenPrintMode printMode) {
  switch (printMode) {

    case TEST_SIMPLE :
      fprintf(output, "%ls: ", t->name);    
      fprintf(output, "idf=%.2f, in docs: %u(tf = %.2f)",
              inverseDocumentFrequency(t),
              t->postingList[0].docId,
              termFrequency(t, t->postingList[0].docId));
      for (unsigned int i = 1; i < t->listLength; ++i) {
        fprintf(output, ", %u(tf = %.2f)",
                t->postingList[i].docId,
                termFrequency(t, t->postingList[i].docId));
      }
      fprintf(output, "\n");
      break;

    case TEST_TFIDF:
      fprintf(output, "%ls: ", t->name);     
      fprintf(output, "%u", t->postingList[0].docId);
      for (unsigned int i = 1; i < t->listLength; ++i) {
        fprintf(output, ", %u", t->postingList[i].docId);
      }
      fprintf(output, "\n");
      break;

    case SERIALIZATION:
      fprintf(output, "%ls\n", t->name);
      fprintf(output, "%u %u", t->postingList[0].docId, t->postingList[0].occurrences);
      for (unsigned int i = 1; i < t->listLength; ++i) {
        fprintf(output, " %u %u", t->postingList[i].docId, t->postingList[i].occurrences);
      }
      fprintf(output, "\n");
      break;
  }
}

  //See token.h
  Token* saveAndCleanVocabulary(Token* vocabulary) {
  static unsigned int indexPartNumber = 1; // Number to use for the filename of the next index part.

  //Sort the hash table to serialize a sorted index
  HASH_SORT(vocabulary, compareTokens);
  Token* t = NULL;
  Token* tmp = NULL;
  char indexPartFileName[20]; //Array to receive the filename of the next index part (5 characters for index, 14 characters max for the number)
  sprintf(indexPartFileName, "index%u", indexPartNumber);
  FILE* indexPart = fopen(indexPartFileName, "w+");
  HASH_ITER(hh, vocabulary, t, tmp) {
    fprintToken(indexPart, t, SERIALIZATION);
  }

  indexPartNumber++;

  //Free the hash table contents
  t = NULL;
  tmp = NULL;
  HASH_ITER(hh, vocabulary, t, tmp) {
    HASH_DEL(vocabulary, t);
    freeToken(t);
    t = NULL;
  }
  return vocabulary;
}

// Tries to add an occurence of the tokenName, docId pair in the vocabulary. Returns NULL if it can't be done
static Token* tryToAddToken(Token* vocabulary, wchar_t* tokenName, unsigned int docId) {
  if (!addOccurrence(docId)) return NULL;

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

    token->postingList = (PostingListEntry*) malloc(sizeof(PostingListEntry)
                                                    * kInitialPostingListSize);
    if (!token->postingList) return NULL;

    token->listLength = 1;
    token->listSize = kInitialPostingListSize;
    token->postingList[0].docId = docId;
    token->postingList[0].occurrences = 1;

    // Add it to the hash map
    HASH_ADD_KEYPTR(hh, vocabulary, token->name,
                    wcslen(token->name) * sizeof(wchar_t), token);
  } else {
    // Found and not duplicate: just adding the docId to the list
    if (token->listLength >= token->listSize) {
      token->listSize *= kSizeAugmentFactor;
      token->postingList = (PostingListEntry*)
                           realloc(token->postingList, token->listSize
                                   * sizeof(PostingListEntry));
      if (!token->postingList) return NULL;
    }

    unsigned int docIdFound;
    // If the document id is in the posting list, just increment occurrences
    for (docIdFound = 0; docIdFound < token->listLength; ++docIdFound) {
      if (token->postingList[docIdFound].docId == docId){
        token->postingList[docIdFound].occurrences++;
        break;
      }
    }

    // If no entry for this term in this document yet, add docId to the list
    if (docIdFound == token->listLength) {
      token->listLength++;
      token->postingList[token->listLength - 1].docId = docId;
      token->postingList[token->listLength - 1].occurrences = 1;
    }
  }

  return vocabulary;
}

// See token.h
Token* addToken(Token* vocabulary, wchar_t* tokenName, unsigned int docId) {
  Token* vocabularyToReturn = vocabulary;
  if((vocabularyToReturn = tryToAddToken(vocabularyToReturn, tokenName, docId)) == NULL) {
    vocabularyToReturn = saveAndCleanVocabulary(vocabularyToReturn);
    vocabularyToReturn = tryToAddToken(vocabularyToReturn, tokenName, docId);
  }
  return vocabularyToReturn;
}

// See token.h
void freeToken(Token* t) {
  free(t->postingList);
  t->postingList = NULL;
  free(t->name);
  t->name = NULL;
  free(t);
  t = NULL;
}
