#include <errno.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "./isn_generation/generate_isn.h"
#include "./lib.h"
#include "./tcp_stack.h"
#include "./utils.h"

tcp_connection *tcp_open(tcp_stack *stack, tcp_socket local_socket,
                         tcp_socket remote_socket, tcp_connection_mode mode) {
  tcp_connection *conn = malloc(sizeof(tcp_connection));
  // TODO - maybe instead of completely quitting here the spec says we should do
  // something else?
  if (conn == NULL) {
    fprintf(stderr, "Failed to malloc connection\n");
    exit(1);
  }

  *conn = (tcp_connection){
      .mode = mode,
      .state = mode == PASSIVE ? LISTEN : SYN_SENT,
      .local_socket = local_socket,
  };
  // For passive connections, we'll fill in these details at a later
  // point (when we receive a SYN).
  if (mode == ACTIVE) {
    conn->remote_socket = remote_socket;
    conn->initial_send_seq_number =
        generate_isn(stack, local_socket, remote_socket);
  }

  // TODO - spec says we should also fill in "Diffserv field,
  // security/compartment, and user timeout information".
  // And also "Verify the security and Diffserv value requested are allowed
  // for this user, if not, return "error: Diffserv value not allowed" or
  // "error: security/compartment not allowed""

  pthread_mutex_lock(&stack->connection_pool.mutex);
  tcp_connection_id connection_id = tcp_connection_get_id(conn);
  hash_map *hash_map_to_add_to =
      mode == PASSIVE ? stack->connection_pool.connections_in_listen_state
                      : stack->connection_pool.connections_not_in_listen_state;

  // TODO - could optimise this with a `hash_map_insert_if_not_already_present`
  // function.
  tcp_connection *existing_connection = hash_map_get(
      hash_map_to_add_to, connection_id.buffer, connection_id.buffer_len);
  if (existing_connection != NULL) {
    fprintf(stderr, "Connection already exists\n");
    exit(1);
  }
  hash_map_insert(hash_map_to_add_to, connection_id.buffer,
                  connection_id.buffer_len, conn);

  if (mode == ACTIVE) {
    // TODO
    // tcp_send_segment(SYN);
  }

  pthread_mutex_unlock(&stack->connection_pool.mutex);
  return conn;
}

tcp_connection *tcp_open_passive(tcp_stack *stack, tcp_socket local_socket) {
  return tcp_open(stack, local_socket, (tcp_socket){0}, PASSIVE);
}

tcp_connection *tcp_open_active(tcp_stack *stack, tcp_socket local_socket,
                                tcp_socket remote_socket) {
  return tcp_open(stack, local_socket, remote_socket, ACTIVE);
}

void tcp_send(tcp_connection *conn, void *data, uint32_t data_size) {
  // TODO
}
