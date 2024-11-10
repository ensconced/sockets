#pragma once

#include "../buffer_state/buffer_state.h"
#include "../request/request.h"
#include "../tcp_socket.h"
#include <stddef.h>
#include <stdint.h>

typedef struct tcp_stack tcp_stack;

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
  tcp_stack *stack;
  tcp_connection_state state;
  tcp_connection_mode mode;
  internal_tcp_socket local_socket;
  internal_tcp_socket remote_socket;
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
  request *connection_request;
  request *receive_request;
} tcp_connection;

buffer tcp_connection_id_create(internal_tcp_socket local_socket, internal_tcp_socket remote_socket);
void tcp_connection_id_destroy(buffer connection_id);
