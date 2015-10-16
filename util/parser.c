#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "util/parser.h"

#include "util/types.h"

const unsigned int kBufferSize = 128;

static const wchar_t kTokenSeparator = L' ';

// Moves some part of the buffer left, overwriting what is before offset, and
// places the rest of the input stream at the leftover space.
// Example: kTokenSeparator = L'-', kBufferSize = 10, offset = 4
// input (already read): <--- e  b  j  -  a  b  -  g  t
// input (left to read): <--- d  \n r  e  s  t  EOF
// buffer = e  b  j  -  a  b  -  g  t  \0
// After this rotate function, buffer becomes:
// +------> a  b  -  g  t  d  \n \0 t  \0
// Note: if there is a L'\n' or EOF character read in the input, it stops the
// read, thus making the buffer smaller (less non L'\0' characters) until buffer
// is empty or contains a single L'\n' character.
static void rotate(wchar_t* buffer, unsigned int offset, FILE* input) {
  if (!offset) return;

  // Number of characters to move left
  unsigned int remainingSize = kBufferSize - offset - 1;

  // Moving the characters left
  wcsncpy(buffer, buffer + offset, remainingSize);
  buffer[remainingSize] = L'\0';

  // Filling the empty space with the rest of the input to read
  if (L'\n' != buffer[wcslen(buffer) - 1]) {
    fgetws(buffer + remainingSize, offset + 1, input);
  }
}

// See util/parser.h
bool nextToken(wchar_t* buffer, FILE* input,
               wchar_t* tokenName, bool* endOfBuffer) {
  if (!buffer || !*buffer || L'\n' == *buffer) {
    *endOfBuffer = true;  // No more to read
    return true;
  }

  unsigned int splitIndex = 0;
  for ( ; splitIndex < kBufferSize; ++splitIndex) {
    if (kTokenSeparator == buffer[splitIndex]) break;  // Found
    if (L'\n' == buffer[splitIndex] ||
      !buffer[splitIndex]) splitIndex = kBufferSize;  // Not found
  }

  // Extracting the characters before splitIndex
  wcsncpy(tokenName, buffer, splitIndex);

  // Adding the end of string (L'\0') character (removing trailing L'\n')
  unsigned int lastCharIndex = wcslen(tokenName) - 1;
  if (lastCharIndex < kBufferSize && L'\n' == tokenName[lastCharIndex]) {
    tokenName[lastCharIndex] = L'\0';
  } else {
    tokenName[splitIndex] = L'\0';
  }

  // No split character found: this is the last token
  if (splitIndex >= kBufferSize) {
    *endOfBuffer = true;
    return true;
  }

  // Removing token from buffer + adding more of the input stream in buffer
  rotate(buffer, splitIndex + 1, input);

  // Null buffer, empty buffer or L'\n' buffer all mean no next token
  *endOfBuffer = !buffer || !*buffer || L'\n' == *buffer;

  return true;
}
