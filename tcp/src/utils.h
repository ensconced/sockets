#pragma once

#include <stdio.h>
#include <string.h>

// TODO - bounds checking?
#define push_value(ptr, value)                                                 \
  do {                                                                         \
    memcpy(ptr, &value, sizeof(value));                                        \
    ptr += sizeof(value);                                                      \
  } while (0)

#define take_value(vec, ptr, value)                                            \
  do {                                                                         \
    if (ptr + sizeof(value) <= vec.buffer + vec.len) {                         \
      memcpy(&value, ptr, sizeof(value));                                      \
      ptr += sizeof(value);                                                    \
    } else {                                                                   \
      fprintf(stderr, "Attempted to take value from outside buffer bounds\n"); \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

typedef struct vec {
  uint8_t *buffer;
  size_t len;
} vec;
