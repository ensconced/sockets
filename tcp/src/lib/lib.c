#include "./lib.h"
#include "../constants.h"
#include "../error_handling/error_handling.h"
#include "../isn_generation/generate_isn.h"
#include "../process_event/process_event.h"
#include "../send_segment/send_segment.h"
#include <arpa/inet.h>
#include <errno.h>

tcp_stack *sockets_create_stack() {
  tcp_stack *stack = tcp_stack_create();
  tcp_stack_start(stack);
  return stack;
}

void sockets_destroy_stack(tcp_stack *stack) { tcp_stack_destroy(stack); }

static uint32_t host_order_ipv4_addr(char *addr) {
  uint32_t network_order_addr;

  switch (inet_pton(AF_INET, addr, &network_order_addr)) {
  case 1: {
    return ntohl(network_order_addr);
  }
  case 0: {
    fprintf(stderr, "Address %s is invalid\n", addr);
    exit(1);
  }
  case -1: {
    fprintf(stderr, "Failed to convert address: %s\n", strerror(errno));
    exit(1);
  }
  default: {
    fprintf(stderr, "Unexpected error converting address\n");
    exit(1);
  }
  };
}

tcp_connection *sockets_open_connection(tcp_stack *stack, sockets_open_opts opts) {
  tcp_connection *conn = checked_malloc(sizeof(tcp_connection), "tcp_connection");

  internal_tcp_socket local_socket = {
      .host_order_ipv4_addr = host_order_ipv4_addr(opts.local_address),
      .host_order_port = opts.local_port,
  };
  internal_tcp_socket remote_socket = {
      .host_order_ipv4_addr = host_order_ipv4_addr(opts.remote_address),
      .host_order_port = opts.remote_port,
  };

  uint32_t isn = generate_isn(stack, local_socket, remote_socket);

  *conn = (tcp_connection){
      .mode = ACTIVE,
      .state = SYN_SENT,
      .local_socket = local_socket,
      .remote_socket = remote_socket,
      .initial_send_sequence_number = isn,
      .send_next = isn + 1,
  };

  tcp_connection_pool_add(stack->connection_pool, conn);
  tcp_send_segment(stack, conn, NULL, 0, SYN);
  conn->state = SYN_SENT;
  return conn;
}