#include <netinet/ip.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "./lib.h"

#define MAX_CONNECTIONS 256
#define RAW_SOCKET_SEND_BUFFER_LEN 65536

tcp_connection *tcp_open_passive(tcp_stack *stack, tcp_socket local_socket) {
  tcp_connection *conn = NULL;
  pthread_mutex_lock(&stack->connections.mutex);
  for (int i = 0; i < MAX_CONNECTIONS; i++) {
    conn = &stack->connections.buffer[i];
    if (conn->state == CLOSED) {
      // TODO - spec says we should also fill in "Diffserv field,
      // security/compartment, and user timeout information".
      // And also "Verify the security and Diffserv value requested are allowed
      // for this user, if not, return "error: Diffserv value not allowed" or
      // "error: security/compartment not allowed""
      conn->mode = PASSIVE;
      conn->state = LISTEN;
      conn->local_socket = local_socket;
      // We'll fill in the remote socket at a later point (when we receive a
      // SYN).
      conn->remote_socket = (tcp_socket){};
      break;
    }
  }
  pthread_mutex_unlock(&stack->connections.mutex);
  if (conn) {
    return conn;
  }
  // TODO - maybe instead of completely quitting here the spec says we should do
  // something else?
  fprintf(stderr, "Failed to open connection\n");
  exit(1);
}

tcp_connection *tcp_open_active(tcp_stack *stack, tcp_socket local_socket,
                                tcp_socket remote_socket) {
  tcp_connection *conn = NULL;
  pthread_mutex_lock(&stack->connections.mutex);
  for (int i = 0; i < MAX_CONNECTIONS; i++) {
    conn = &stack->connections.buffer[i];
    if (conn->state == CLOSED) {
      // TODO - spec says we should also fill in "Diffserv field,
      // security/compartment, and user timeout information".
      // And also "Verify the security and Diffserv value requested are allowed
      // for this user, if not, return "error: Diffserv value not allowed" or
      // "error: security/compartment not allowed""
      conn->mode = ACTIVE;
      conn->state = SYN_SENT;
      conn->local_socket = local_socket;
      conn->remote_socket = remote_socket;
      break;
    }
  }

  if (conn) {
    tcp_send_segment(SYN);
  }

  pthread_mutex_unlock(&stack->connections.mutex);

  if (conn) {
    return conn;
  }

  // TODO - maybe instead of completely quitting here the spec says we should do
  // something else?
  fprintf(stderr, "Failed to open connection\n");
  exit(1);
}

void tcp_send(tcp_connection *conn) {}

tcp_stack tcp_init(void) {
  tcp_connection *connections_buffer =
      malloc(sizeof(tcp_connection) * MAX_CONNECTIONS);
  if (connections_buffer == 0) {
    fprintf(stderr, "Failed to malloc connections buffer\n");
    exit(1);
  }
  for (int i = 0; i < MAX_CONNECTIONS; i++) {
    connections_buffer[i] = (tcp_connection){.mode = PASSIVE, .state = CLOSED};
  }

  pthread_mutex_t mutex;
  pthread_mutex_init(&mutex, NULL);
  tcp_connections conns = {
      .buffer = connections_buffer, .length = 0, .mutex = mutex};

  // pthread_t incoming_datagram_handler_thread_id;
  // if (pthread_create(&incoming_datagram_handler_thread_id, NULL,
  //                    handle_incoming_datagrams, connections) != 0) {
  //   fprintf(stderr, "Failed to create datagram handling thread\n");
  //   exit(1);
  // };

  // pthread_t timeout_handler_thread_id;
  // if (pthread_create(&timeout_handler_thread_id, NULL, handle_timeouts,
  //                    connections) != 0) {
  //   fprintf(stderr, "Failed to create timeout handling thread\n");
  //   exit(1);
  // };

  int ip_sock_fd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
  if (ip_sock_fd == -1) {
    fprintf(stderr, "Failed to create socket\n");
    exit(1);
  }

  pthread_mutex_t socket_mutex;
  pthread_mutex_init(&socket_mutex, NULL);
  uint8_t *socket_send_buffer =
      malloc(RAW_SOCKET_SEND_BUFFER_LEN * sizeof(uint8_t));

  return (tcp_stack){
      .connections = conns,
      .raw_socket =
          (tcp_raw_socket){
              .fd = ip_sock_fd,
              .mutex = socket_mutex,
              .send_buffer = socket_send_buffer,
          },
  };
}
