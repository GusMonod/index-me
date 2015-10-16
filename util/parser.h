#ifndef UTIL_PARSER_H_
#define UTIL_PARSER_H_

#include <stdio.h>

#include "util/types.h"

// Size of the buffer used for parsing the input.
// It includes \0, \n, and at least one separator so max size of
// a word to parse is kBufferSize - 3.
extern const unsigned int kBufferSize;

// Returns true if there is more to read, false otherwise.
bool endOfBuffer(wchar_t* buffer);

// Populates the wide character string to a token name.
// Returns false if there was an error reading, true otherwise.
bool nextToken(wchar_t* buffer, FILE* input, wchar_t* tokenName);

#endif  // UTIL_PARSER_H_
