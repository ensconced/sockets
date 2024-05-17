#include "./defines.h"
#include "./lib.h"
#include <stdlib.h>
#include <string.h>

int main(void) {
  tcp_stack *stack = tcp_stack_create();
  tcp_socket local_socket = {.ipv4_addr = 127 << 24 | 1, .port = LOCAL_PORT};
  tcp_socket remote_socket = {.ipv4_addr = 127 << 24 | 1, .port = REMOTE_PORT};
  tcp_connection *conn = tcp_open_active(stack, local_socket, remote_socket);
  char *data = "GET http://localhost:3000/test HTTP/1.1\n\n";
  tcp_send(conn, data, (uint32_t)strlen(data));
  while (1)
    ;
  tcp_stack_destroy(stack);
}
