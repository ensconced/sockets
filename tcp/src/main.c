#include "./lib/lib.h"
#include <stdio.h>

int main() {
  tcp_stack *stack = sockets_create_stack();

  sockets_open_opts connection_opts = {
      .local_address = "192.168.111.221",
      .local_port = 1234,
      .remote_address = "1.1.1.1",
      .remote_port = 80,
      .timeout_ms = 1000,
      .mode = SOCKET_OPEN_MODE_ACTIVE,
  };
  tcp_connection *connection = sockets_open_connection(stack, connection_opts);

  char *http_request = "GET / HTTP/1.0\r\n\r\n";
  sockets_send_opts send_opts = {
      .connection = connection,
      .buffer = (void *)http_request,
      .byte_count = strlen(http_request),
  };
  sockets_send(stack, send_opts);

  sockets_destroy_stack(stack);
}
