#ifndef UTIL_PARSER_H_
#define UTIL_PARSER_H_

#include "util/types.h"

// Size of the buffer used for parsing the input.
// It includes \0, \n, and at least one separator so max size of
// a word to parse is kBufferSize - 3.
extern const unsigned int kBufferSize;

// Populates the wide character string to a token name.
// The parameter done is set to false if there is more to read, true otherwise.
// Returns false if there was an error reading, true otherwise.
bool nextToken(wchar_t* buffer, FILE* input, wchar_t* tokenName, bool* done);

#endif  // UTIL_PARSER_H_
