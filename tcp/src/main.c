#include "./lib.h"

int main(void) {
  tcp_stack stack = tcp_init();
  tcp_socket local_socket = {.ipv4_addr = 0, .port = 3001};
  tcp_socket remote_socket = {.ipv4_addr = 127 << 24 & 1, .port = 3000};
  tcp_connection *conn = tcp_open_active(&stack, local_socket, remote_socket);
  tcp_send(conn, "hello there...");
}
