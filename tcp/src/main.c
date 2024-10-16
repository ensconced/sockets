#include "./lib/lib.h"
#include <stdio.h>

int main() {
  tcp_stack *stack = sockets_create_stack();
  sockets_open_opts connection_opts = {
      .local_address = "192.178.168.222",
      .local_port = 1234,
      .remote_address = "8.8.8.8",
      .remote_port = 80,
      .timeout_ms = 1000,
      .mode = SOCKET_OPEN_MODE_ACTIVE,
  };
  tcp_connection *connection = sockets_open_connection(stack, connection_opts);
  sockets_destroy_stack(stack);
}
