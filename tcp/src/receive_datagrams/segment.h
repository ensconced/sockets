#pragma once

#include "../buffer_state/buffer_state.h"
#include <stdint.h>

typedef struct tcp_segment {
  uint16_t source_port;
  uint16_t dest_port;
  uint32_t sequence_number;
  uint32_t acknowledgement_number;
  uint8_t flags;
  uint16_t window;
  uint16_t checksum;
  uint16_t urgent_pointer;
  buffer options;
  buffer data;
} tcp_segment;
