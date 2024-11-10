#include "./tcp_connection.h"
#include "../buffer_state/buffer_state.h"
#include "../error_handling/error_handling.h"
#include "../tcp_socket.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

buffer tcp_connection_id_create(internal_tcp_socket local_socket, internal_tcp_socket remote_socket) {
  size_t byte_count = sizeof(local_socket.host_order_ipv4_addr) + sizeof(local_socket.host_order_port) +
                      sizeof(remote_socket.host_order_ipv4_addr) + sizeof(remote_socket.host_order_port);
  uint8_t *data = checked_malloc(byte_count, "tcp connection id buffer");
  buffer buf = {.data = data, .size_bytes = byte_count};
  buffer_state writer = {.buffer = buf, .ptr = data};
  push_uint32_t(&writer, local_socket.host_order_ipv4_addr);
  push_uint16_t(&writer, local_socket.host_order_port);
  push_uint32_t(&writer, remote_socket.host_order_ipv4_addr);
  push_uint16_t(&writer, remote_socket.host_order_port);
  return buf;
}

void tcp_connection_id_destroy(buffer connection_id) { free(connection_id.data); }
