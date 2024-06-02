#include "./tcp_connection.h"
#include "../error_handling/error_handling.h"
#include "../tcp_socket.h"
#include "../utils.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

vec tcp_connection_id_create(tcp_socket local_socket,
                             tcp_socket remote_socket) {
  size_t byte_count =
      sizeof(local_socket.ipv4_addr) + sizeof(local_socket.port) +
      sizeof(remote_socket.ipv4_addr) + sizeof(remote_socket.port);
  uint8_t *buffer = checked_malloc(byte_count, "tcp connection id buffer");
  vec result = {
      .buffer = buffer,
      .len = byte_count,
  };
  uint8_t *ptr = buffer;
  push_uint32_t(result, &ptr, local_socket.ipv4_addr);
  push_uint16_t(result, &ptr, local_socket.port);
  push_uint32_t(result, &ptr, remote_socket.ipv4_addr);
  push_uint16_t(result, &ptr, remote_socket.port);
  return result;
}

void tcp_connection_id_destroy(vec connection_id) {
  free(connection_id.buffer);
}
