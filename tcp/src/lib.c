#include <errno.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "./constants.h"
#include "./error_handling/error_handling.h"
#include "./isn_generation/generate_isn.h"
#include "./send_segment/send_segment.h"
#include "./tcp_connection/tcp_connection_pool.h"
#include "./tcp_stack.h"
#include "./utils.h"
#include <arpa/inet.h>

internal_tcp_socket internalize_socket(tcp_socket socket) {
  uint32_t network_order_ipv4_addr;
  int res = inet_pton(AF_INET, socket.ipv4_addr, &network_order_ipv4_addr);
  if (res == 0) {
    fprintf(stderr, "Invalid address string\n");
    exit(1);
  }
  if (res == -1) {
    fprintf(stderr, "Invalid address family: %s\n", strerror(errno));
    exit(1);
  }
  return (internal_tcp_socket){
      .host_order_port = socket.port,
      .host_order_ipv4_addr = ntohl(network_order_ipv4_addr),
  };
}

tcp_connection *tcp_open_passive(tcp_stack *stack, tcp_socket local_socket) {
  tcp_connection *conn = checked_malloc(sizeof(tcp_connection), "connection");

  *conn = (tcp_connection){
      .mode = PASSIVE,
      .state = LISTEN,
      .local_socket = internalize_socket(local_socket),
  };

  pthread_mutex_lock(stack->connection_pool.mutex);
  tcp_connection_pool_add(stack->connection_pool, conn);
  pthread_mutex_unlock(stack->connection_pool.mutex);
  return conn;
}

tcp_connection *tcp_open_active(tcp_stack *stack, tcp_socket local_socket,
                                tcp_socket remote_socket) {

  tcp_connection *conn =
      checked_malloc(sizeof(tcp_connection), "tcp_connection");

  uint32_t isn = generate_isn(stack, local_socket, remote_socket);

  *conn = (tcp_connection){
      .mode = ACTIVE,
      .state = SYN_SENT,
      .local_socket = internalize_socket(local_socket),
      .remote_socket = internalize_socket(remote_socket),
      .initial_send_sequence_number = isn,
      .send_next = isn + 1,
  };

  pthread_mutex_lock(stack->connection_pool.mutex);

  tcp_connection_pool_add(stack->connection_pool, conn);
  tcp_send_segment(stack, conn, NULL, 0, SYN);
  conn->state = SYN_SENT;

  pthread_mutex_unlock(stack->connection_pool.mutex);
  return conn;
}

// TODO - eventually this will just push some data onto a buffer, to eventually
// be sent by the underlying implementation, which will support re-sending etc.
// but for now, it literally just sends the segment.
void tcp_send(tcp_stack *stack, tcp_connection *conn, void *data,
              size_t data_size) {
  tcp_send_segment(stack, conn, data, data_size, ACK);
}
