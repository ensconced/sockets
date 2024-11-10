#include "./lib.h"
#include "../constants.h"
#include "../error_handling/error_handling.h"
#include "../isn_generation/generate_isn.h"
#include "../process_event/process_event.h"
#include "../request/request.h"
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

  request *connection_request = request_create();

  *conn = (tcp_connection){
      .stack = stack,
      .mode = ACTIVE,
      .state = SYN_SENT,
      .local_socket = local_socket,
      .remote_socket = remote_socket,
      .initial_send_sequence_number = isn,
      .send_next = isn + 1,
      .connection_request = connection_request,
  };

  tcp_connection_pool_add(stack->connection_pool, conn);
  tcp_send_segment(stack, conn, NULL, 0, SYN);
  conn->state = SYN_SENT;
  request_block_until_resolved(connection_request);
  request_destroy(connection_request);
  return conn;
}

void sockets_send(tcp_connection *connection, sockets_send_opts opts) {
  // TODO - eventually this will just push some data onto a buffer, to eventually
  // be sent by the underlying implementation, which will support re-sending etc.
  // but for now, it literally just sends the segment.
  tcp_send_segment(connection->stack, connection, opts.buffer, opts.byte_count, ACK);
}

void sockets_receive(tcp_connection *connection, sockets_receive_opts opts) {
  request *receive_request = request_create();
  connection->receive_request = receive_request;
  request_block_until_resolved(receive_request);
  printf("request resolved!\n");
  request_destroy(receive_request);
}
