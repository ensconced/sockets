#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct buffer {
  void *data;
  size_t size_bytes;
} buffer;

typedef struct buffer_state {
  buffer buffer;
  void *ptr;
} buffer_state;

void push_uint8_t(buffer_state *writer, uint8_t value);
void push_uint16_t(buffer_state *writer, uint16_t value);
void push_uint32_t(buffer_state *writer, uint32_t value);

uint8_t take_uint8_t(buffer_state *reader);
uint16_t take_uint16_t(buffer_state *reader);
uint32_t take_uint32_t(buffer_state *reader);
