#pragma once

#define RAW_SOCKET_SEND_BUFFER_LEN 65536
#define RAW_SOCKET_RECEIVE_BUFFER_LEN 65536

#include <stdint.h>

typedef struct {
  char *ipv4_addr;
  uint16_t port;
} tcp_socket;

typedef struct {
  uint32_t host_order_ipv4_addr;
  uint16_t host_order_port;
} internal_tcp_socket;
