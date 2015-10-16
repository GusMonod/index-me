#include <stdio.h>
#include <wchar.h>

#include "util/types.h"

static const unsigned int kBufferSize = 32;

static bool rotate(wchar_t* buffer, unsigned int offset, FILE* input) {
  return 0;
}

static bool read(wchar_t* buffer, unsigned int size, FILE* input) {
  return 0;
}

int main(int argc, char **argv) {
  FILE* input = stdin;

  // Allocation of the unicode buffer
  wchar_t* buffer = (wchar_t*) malloc(sizeof(wchar_t) * kBufferSize);

  // Reading the first bytes
  if (!read(buffer, kBufferSize, input)) return 1;

  unsigned int i = 0;
  // As long as the buffer is not empty
  while (buffer && *buffer) {
    // Rotate buffer if needed
    if (i + 10 >= kBufferSize) {
      if (!rotate(buffer, i, input)) return 1;
      i = 0;
    } else {  // Print 10 unicode characters:
      printf("%ls", buffer + i);
      i += 10;
    }
  }

  return 0;
}
