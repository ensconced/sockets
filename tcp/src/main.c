#include "./config.h"
#include "./lib.h"
#include <stdlib.h>
#include <string.h>

int main(void) {
  tcp_stack *stack = tcp_stack_create();
  tcp_socket local_socket = {.ipv4_addr = LOCAL_IP, .port = 3001};
  tcp_socket remote_socket = {.ipv4_addr = REMOTE_IP, .port = 3000};
  tcp_connection *conn = tcp_open_active(stack, local_socket, remote_socket);
  char *data = "GET http://192.168.178.30:3000/test HTTP/1.1\n\n";
  tcp_send(conn, data, (uint32_t)strlen(data));
  while (1) {
  };
  tcp_stack_destroy(stack);
}
