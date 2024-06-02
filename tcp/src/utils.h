#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct vec {
  uint8_t *buffer;
  size_t len;
} vec;

void push_uint8_t(vec v, uint8_t **pt, uint8_t value);
void push_uint16_t(vec v, uint8_t **ptr, uint16_t value);
void push_uint32_t(vec v, uint8_t **ptr, uint32_t value);

uint8_t take_uint8_t(vec v, uint8_t **pt);
uint16_t take_uint16_t(vec v, uint8_t **ptr);
uint32_t take_uint32_t(vec v, uint8_t **ptr);
