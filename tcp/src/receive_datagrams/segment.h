#pragma once

#include "../utils.h"
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
  vec options;
  vec data;
} tcp_segment;
