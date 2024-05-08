#include <errno.h>
#include <md5.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>

#include "./lib.h"
#include "./secret_key.h"
#include "./utils.h"

#define MAX_CONNECTIONS 256
#define RAW_SOCKET_SEND_BUFFER_LEN 65536

// ISN_HASH_STRING_LEN is the sum of:
// local ipv4 address:  4 bytes
// local port:          2 bytes
// remote ipv4 address: 4 bytes
// remote port:         2 bytes
// secret key:          16 bytes
#define ISN_HASH_STRING_LEN = 28;

uint32_t get_isn(tcp_socket local_socket, tcp_socket remote_socket) {
  struct timespec time;
  if (clock_gettime(CLOCK_MONOTONIC, &time) != 0) {
    fprintf(stderr, "Error reading clock: %s", strerror(errno));
  }
  uint64_t microseconds = time.tv_sec * 1000 * 1000 + time.tv_nsec / 1000;
  uint32_t fours_of_microseconds = (uint32_t)(microseconds / 4);

  uint8_t hash_string[ISN_HASH_STRING_LEN] = {};
  uint8_t *ptr = hash_string;
  push_value(ptr, local_socket.ipv4_addr);
  push_value(ptr, local_socket.port);
  push_value(ptr, remote_socket.ipv4_addr);
  push_value(ptr, remote_socket.port);
  push_value(ptr, secret_key);

  return fours_of_microseconds + md5(hash_string, ISN_HASH_STRING_LEN);
}

tcp_connection *tcp_open(tcp_stack *stack, tcp_socket local_socket,
                         tcp_socket remote_socket, tcp_connection_mode mode) {
  // TODO - I think we also need to check whether there is already a connection
  // using the same local & remote sockets - because you shouldn't be able to
  // open a new one using the same pair?
  tcp_connection *conn = NULL;
  pthread_mutex_lock(&stack->connection_pool.mutex);
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
        conn->initial_send_seq_number = get_isn(local_socket, remote_socket);
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
    tcp_send_segment(SYN);
  }

  pthread_mutex_unlock(&stack->connection_pool.mutex);
  return conn;
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
  tcp_connection_pool conns = {
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
      .connection_pool = conns,
      .raw_socket =
          (tcp_raw_socket){
              .fd = ip_sock_fd,
              .mutex = socket_mutex,
              .send_buffer = socket_send_buffer,
          },
  };
}
