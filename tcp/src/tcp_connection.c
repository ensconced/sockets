#include "./tcp_connection.h"
#include "./utils.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tcp_connection_id tcp_connection_get_id(tcp_connection *conn) {
  size_t byte_count =
      sizeof(conn->local_socket.ipv4_addr) + sizeof(conn->local_socket.port) +
      sizeof(conn->remote_socket.ipv4_addr) + sizeof(conn->remote_socket.port);
  uint8_t *buffer = malloc(byte_count);
  if (buffer == NULL) {
    fprintf(stderr, "Failed to malloc tcp connection id buffer\n");
    exit(1);
  }
  uint8_t *ptr = buffer;
  push_value(ptr, conn->local_socket.ipv4_addr);
  push_value(ptr, conn->local_socket.port);
  push_value(ptr, conn->remote_socket.ipv4_addr);
  push_value(ptr, conn->remote_socket.port);
  return (tcp_connection_id){
      .buffer = buffer,
      .buffer_len = (size_t)(ptr - buffer),
  };
}
