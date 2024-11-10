#pragma once

#include "../tcp_stack/tcp_stack.h"

tcp_stack *sockets_create_stack();
void sockets_destroy_stack(tcp_stack *stack);

typedef enum socket_open_mode { SOCKET_OPEN_MODE_ACTIVE, SOCKET_OPEN_MODE_PASSIVE } socket_open_mode;
typedef struct sockets_open_opts {
  uint16_t local_port;
  char *local_address;
  uint16_t remote_port;
  char *remote_address;
  socket_open_mode mode;
  uint32_t timeout_ms;
} sockets_open_opts;
tcp_connection *sockets_open_connection(tcp_stack *stack, sockets_open_opts opts);

typedef struct sockets_send_opts {
  uint8_t *buffer;
  size_t byte_count;
  bool urgent;
  bool push;
  uint32_t timeout_ms;
} sockets_send_opts;
void sockets_send(tcp_connection *connection, sockets_send_opts opts);

typedef struct sockets_receive_opts {
  uint8_t *buffer;
  size_t buffer_size_bytes;
} sockets_receive_opts;
typedef struct sockets_receive_result {
  bool urgent;
  bool push;
  size_t bytes_written;
} sockets_receive_result;
void sockets_receive(tcp_connection *connection, sockets_receive_opts opts);

// void sockets_close(tcp_connection *connection);

// typedef struct sockets_status_result {
// } sockets_status_result;
// sockets_status_result sockets_status(tcp_connection *connection);

// void sockets_abort(tcp_connection *connection);
