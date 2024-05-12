#include <errno.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "./constants.h"
#include "./isn_generation/generate_isn.h"
#include "./lib.h"
#include "./tcp_stack.h"
#include "./utils.h"

tcp_connection *tcp_open(tcp_stack *stack, tcp_socket local_socket,
                         tcp_socket remote_socket, tcp_connection_mode mode) {
  // TODO - I think we also need to check whether there is already a connection
  // using the same local & remote sockets - because you shouldn't be able to
  // open a new one using the same pair?

  pthread_mutex_lock(&stack->connection_pool.mutex);
  tcp_connection *conn = NULL;
  for (int i = 0; i < MAX_CONNECTIONS; i++) {
    conn = &stack->connection_pool.buffer[i];
    if (conn->state == CLOSED) {
      // TODO - spec says we should also fill in "Diffserv field,
      // security/compartment, and user timeout information".
      // And also "Verify the security and Diffserv value requested are allowed
      // for this user, if not, return "error: Diffserv value not allowed" or
      // "error: security/compartment not allowed""
      conn->mode = mode;
      conn->state = mode == PASSIVE ? LISTEN : SYN_SENT;
      conn->local_socket = local_socket;
      // For passive connections, we'll fill in these details at a later
      // point (when we receive a SYN).
      if (mode == ACTIVE) {
        conn->remote_socket = remote_socket;
        conn->initial_send_seq_number =
            generate_isn(stack, local_socket, remote_socket);
      }
      break;
    }
  }

  // TODO - maybe instead of completely quitting here the spec says we should do
  // something else?
  if (conn == 0) {
    fprintf(stderr, "Failed to open connection\n");
    exit(1);
  }

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
