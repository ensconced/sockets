#include "./config.h"
#include "./lib.h"
#include <stdlib.h>
#include <string.h>

int main(void) {
  tcp_stack *stack = tcp_stack_create();
  tcp_socket local_socket = {.ipv4_addr = LOCAL_IP, .port = 48442};
  tcp_socket remote_socket = {.ipv4_addr = REMOTE_IP, .port = 80};
  tcp_connection *conn = tcp_open_active(stack, local_socket, remote_socket);
  // char *data = "GET http://192.168.178.30:3000/test HTTP/1.1\n\n";
  uint8_t data[16] = {
      0x04, 0x02, 0x08, 0x0a, 0xb5, 0x42, 0x64, 0x84,
      0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x07,
  };
  tcp_send(conn, data, 16);
  while (1) {
  };
  tcp_stack_destroy(stack);
}
