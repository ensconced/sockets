#pragma once

#include <stdint.h>

typedef struct {
  uint32_t ipv4_addr;
  uint16_t port;
} tcp_socket;
