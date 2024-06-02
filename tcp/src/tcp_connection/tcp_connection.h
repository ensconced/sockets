#pragma once

#include "../tcp_socket.h"
#include "../utils.h"
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
  uint32_t send_unacknowledged;
  uint32_t send_next;
  uint32_t send_window;
  uint32_t send_urgent_pointer;
  uint32_t segment_sequence_number_for_last_window_update;
  uint32_t segment_acknowledgement_number_for_last_window_update;
  uint32_t initial_send_sequence_number;
  uint32_t receive_next;
  uint32_t receive_window;
  uint32_t receive_urgent_pointer;
  uint32_t initial_receive_sequence_number;
} tcp_connection;

vec tcp_connection_id_create(tcp_socket local_socket, tcp_socket remote_socket);
void tcp_connection_id_destroy(vec connection_id);
