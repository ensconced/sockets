#include "./config.h"
#include "./lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  tcp_stack *stack = tcp_stack_create();
  tcp_socket local_socket = {.ipv4_addr = LOCAL_IP, .port = 48442};
  tcp_socket remote_socket = {.ipv4_addr = REMOTE_IP, .port = 80};
  tcp_connection *conn = tcp_open_active(stack, local_socket, remote_socket);

  for (unsigned int i = 0; i < 429496729; i++)
    ;
  char *data = "GET / HTTP/1.1\n\n";
  tcp_send(stack, conn, data, strlen(data));
  for (unsigned int i = 0; i < 4294967295; i++)
    ;
  tcp_stack_destroy(stack);
}
