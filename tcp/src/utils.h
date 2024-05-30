#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// TODO - bounds checking?
#define push_value(ptr, value)                                                 \
  do {                                                                         \
    memcpy(ptr, &value, sizeof(value));                                        \
    ptr += sizeof(value);                                                      \
  } while (0)

typedef struct vec {
  uint8_t *buffer;
  size_t len;
} vec;

void take_uint8_t(vec v, uint8_t **ptr, uint8_t *value_result);
void take_uint16_t(vec v, uint8_t **ptr, uint16_t *value_result);
void take_uint32_t(vec v, uint8_t **ptr, uint32_t *value_result);
