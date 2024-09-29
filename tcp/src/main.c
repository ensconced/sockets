#include "./config.h"
#include "./tcp_stack.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  tcp_stack_create();

  // TODO - this testing code is done directly in `main` for now. But
  // eventually, this will become a daemon process whose only responsibility it
  // to create the tcp stack, and to handle "actions" incoming on a unix domain
  // socket, sent via a netcat-like CLI.

  // tcp_socket local_socket = {.ipv4_addr = LOCAL_IP, .port = 48442};
  // tcp_socket remote_socket = {.ipv4_addr = REMOTE_IP, .port = 80};
  // tcp_connection *conn = tcp_open_active(stack, local_socket, remote_socket);
  // // TODO - this is an absolute hack. Why is it necessary? I guess it's
  // because
  // // currently tcp_send literally just directly sends the TCP segment,
  // instead
  // // of enqueuing it or something???
  // for (unsigned int i = 0; i < 429496729; i++)
  //   ;
  // char *data = "GET / HTTP/1.1\n\n";
  // tcp_send(stack, conn, data, strlen(data));
  // for (unsigned int i = 0; i < 4294967295; i++)
  //   ;
}
