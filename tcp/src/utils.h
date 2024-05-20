#pragma once

#include <string.h>

#define push_value(ptr, value)                                                 \
  do {                                                                         \
    memcpy(ptr, &value, sizeof(value));                                        \
    ptr += sizeof(value);                                                      \
  } while (0)

#define take_value(ptr, value)                                                 \
  do {                                                                         \
    memcpy(&value, ptr, sizeof(value));                                        \
    ptr += sizeof(value);                                                      \
  } while (0)

typedef struct vec {
  void *ptr;
  size_t len;
} vec;
