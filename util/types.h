#ifndef UTIL_TYPES_H_
#define UTIL_TYPES_H_

typedef enum {
  SUCCESS = 0,
  INPUT_ERROR = 1,
  PARSE_ERROR = 2,
  PURGE_ERROR = 3,
  ADD_ERROR = 4
} ReturnCode;

typedef enum bool {false = 0, true = 1} bool;

#endif  // UTIL_TYPES_H_
