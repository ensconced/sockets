#include "./buffer_state.h"
#include <stdint.h>
#include <stdlib.h>

#define GENERIC_TAKE(type)                                                                                             \
  type take_##type(buffer_state *reader) {                                                                             \
    char *new_ptr = (char *)reader->ptr + sizeof(type);                                                                \
    char *buffer_end = (char *)reader->buffer.data + reader->buffer.size_bytes;                                        \
    if (new_ptr <= buffer_end) {                                                                                       \
      type result;                                                                                                     \
      memcpy(&result, reader->ptr, sizeof(type));                                                                      \
      reader->ptr = new_ptr;                                                                                           \
      return result;                                                                                                   \
    } else {                                                                                                           \
      fprintf(stderr, "Attempted to take from outside buffer bounds\n");                                               \
      exit(1);                                                                                                         \
    }                                                                                                                  \
  }

GENERIC_TAKE(uint8_t)
GENERIC_TAKE(uint16_t)
GENERIC_TAKE(uint32_t)

#define GENERIC_PUSH(type)                                                                                             \
  void push_##type(buffer_state *writer, type value) {                                                                 \
    char *new_ptr = (char *)writer->ptr + sizeof(type);                                                                \
    char *buffer_end = (char *)writer->buffer.data + writer->buffer.size_bytes;                                        \
    if (new_ptr <= buffer_end) {                                                                                       \
      memcpy(writer->ptr, &value, sizeof(type));                                                                       \
      writer->ptr = new_ptr;                                                                                           \
    } else {                                                                                                           \
      fprintf(stderr, "Attempted to push outside buffer bounds\n");                                                    \
      exit(1);                                                                                                         \
    }                                                                                                                  \
  }

GENERIC_PUSH(uint8_t)
GENERIC_PUSH(uint16_t)
GENERIC_PUSH(uint32_t)
