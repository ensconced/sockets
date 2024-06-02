#include "./utils.h"
#include <stdint.h>
#include <stdlib.h>

#define GENERIC_TAKE(type)                                                     \
  type take_##type(vec v, uint8_t **ptr) {                                     \
    if (*ptr + sizeof(type) <= v.buffer + v.len) {                             \
      type result;                                                             \
      memcpy(&result, *ptr, sizeof(type));                                     \
      *ptr += sizeof(type);                                                    \
      return result;                                                           \
    } else {                                                                   \
      fprintf(stderr, "Attempted to take from outside buffer bounds\n");       \
      exit(1);                                                                 \
    }                                                                          \
  }

GENERIC_TAKE(uint8_t)
GENERIC_TAKE(uint16_t)
GENERIC_TAKE(uint32_t)

#define GENERIC_PUSH(type)                                                     \
  void push_##type(vec v, uint8_t **ptr, type value) {                         \
    if (*ptr + sizeof(type) <= v.buffer + v.len) {                             \
      memcpy(*ptr, &value, sizeof(type));                                      \
      *ptr += sizeof(type);                                                    \
    } else {                                                                   \
      fprintf(stderr, "Attempted to push outside buffer bounds\n");            \
      exit(1);                                                                 \
    }                                                                          \
  }

GENERIC_PUSH(uint8_t)
GENERIC_PUSH(uint16_t)
GENERIC_PUSH(uint32_t)
