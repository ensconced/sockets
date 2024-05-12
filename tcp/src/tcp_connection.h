#pragma once

#include "./tcp_socket.h"
#include <stddef.h>
#include <stdint.h>

typedef enum tcp_conection_mode { PASSIVE, ACTIVE } tcp_connection_mode;

typedef enum tcp_connection_state {
  CLOSED,
  LISTEN,
  SYN_RECEIVED,
  SYN_SENT,
  ESTABLISHED,
  FIN_WAIT_1,
  FIN_WAIT_2,
  CLOSE_WAIT,
  CLOSING,
  LAST_ACK,
  TIME_WAIT
} tcp_connection_state;

typedef struct tcp_connection {
  tcp_connection_state state;
  tcp_connection_mode mode;
  tcp_socket local_socket;
  tcp_socket remote_socket;
  uint32_t initial_send_seq_number;
} tcp_connection;

typedef struct tcp_connection_id {
  uint8_t *buffer;
  size_t buffer_len;
} tcp_connection_id;

tcp_connection_id tcp_connection_id_create(tcp_socket local_socket,
                                           tcp_socket remote_socket);
void tcp_connection_id_destroy(tcp_connection_id connection_id);
