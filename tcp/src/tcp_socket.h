#pragma once

#define RAW_SOCKET_SEND_BUFFER_LEN 65536
#define RAW_SOCKET_RECEIVE_BUFFER_LEN 65536

#include <stdint.h>

typedef struct {
  uint32_t ipv4_addr;
  uint16_t port;
} tcp_socket;
