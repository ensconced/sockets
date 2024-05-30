#include "./tcp_connection.h"
#include "../error_handling/error_handling.h"
#include "../utils.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tcp_connection_id tcp_connection_id_create(tcp_socket local_socket,
                                           tcp_socket remote_socket) {
  size_t byte_count =
      sizeof(local_socket.ipv4_addr) + sizeof(local_socket.port) +
      sizeof(remote_socket.ipv4_addr) + sizeof(remote_socket.port);
  uint8_t *buffer = checked_malloc(byte_count, "tcp connection id buffer");
  uint8_t *ptr = buffer;
  push_value(ptr, local_socket.ipv4_addr);
  push_value(ptr, local_socket.port);
  push_value(ptr, remote_socket.ipv4_addr);
  push_value(ptr, remote_socket.port);
  return (tcp_connection_id){
      .buffer = buffer,
      .buffer_len = (size_t)(ptr - buffer),
  };
}

void tcp_connection_id_destroy(tcp_connection_id connection_id) {
  free(connection_id.buffer);
}
