#include "./utils.h"
#include <stdint.h>
#include <stdlib.h>

// TODO - this should just return the value instead of using a result param
#define GENERIC_TAKE(type)                                                     \
  void take_##type(vec v, uint8_t **ptr, type *value_result) {                 \
    if (*ptr + sizeof(type) <= v.buffer + v.len) {                             \
      memcpy(value_result, *ptr, sizeof(type));                                \
      *ptr += sizeof(type);                                                    \
    } else {                                                                   \
      fprintf(stderr, "Attempted to take from outside buffer bounds\n");       \
      exit(1);                                                                 \
    }                                                                          \
  }

GENERIC_TAKE(uint8_t)
GENERIC_TAKE(uint16_t)
GENERIC_TAKE(uint32_t)
