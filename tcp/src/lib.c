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
#include "./send_segment/send_segment.h"
#include "./tcp_connection/tcp_connection_pool.h"
#include "./tcp_stack.h"
#include "./utils.h"

tcp_connection *tcp_open_passive(tcp_stack *stack, tcp_socket local_socket) {
  tcp_connection *conn = malloc(sizeof(tcp_connection));
  if (conn == NULL) {
    fprintf(stderr, "Failed to malloc connection\n");
    exit(1);
  }

  *conn = (tcp_connection){
      .mode = PASSIVE,
      .state = LISTEN,
      .local_socket = local_socket,
  };

  pthread_mutex_lock(stack->connection_pool.mutex);
  tcp_connection_pool_add(stack->connection_pool, conn);
  pthread_mutex_unlock(stack->connection_pool.mutex);
  return conn;
}

tcp_connection *tcp_open_active(tcp_stack *stack, tcp_socket local_socket,
                                tcp_socket remote_socket) {
  tcp_connection *conn = malloc(sizeof(tcp_connection));
  if (conn == NULL) {
    fprintf(stderr, "Failed to malloc connection\n");
    exit(1);
  }

  uint32_t isn = generate_isn(stack, local_socket, remote_socket);

  *conn = (tcp_connection){
      .mode = ACTIVE,
      .state = SYN_SENT,
      .local_socket = local_socket,
      remote_socket = remote_socket,
      .initial_send_sequence_number = isn,
      .send_next = isn + 1,
  };

  pthread_mutex_lock(stack->connection_pool.mutex);

  tcp_connection_pool_add(stack->connection_pool, conn);
  tcp_send_segment(stack, conn, NULL, 0, SYN, isn, 0);
  conn->state = SYN_SENT;

  pthread_mutex_unlock(stack->connection_pool.mutex);
  return conn;
}

void tcp_send(tcp_connection *conn, void *data, uint32_t data_size) {
  // TODO
}
