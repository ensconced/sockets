
#include "../tcp_stack.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>

void *receive_datagrams(tcp_stack *stack) {
  while (!atomic_load(stack->destroyed)) {
    struct sockaddr remote_addr;
    socklen_t remote_addr_len;
    ssize_t bytes_received = recvfrom(
        stack->raw_socket.fd, stack->raw_socket.receive_buffer,
        RAW_SOCKET_RECEIVE_BUFFER_LEN, 0, &remote_addr, &remote_addr_len);
    printf("received %zu bytes from addr %d", bytes_received, remote_addr_len);
    for (ssize_t i = 0; i < bytes_received; i++) {
      printf("0x%x ", stack->raw_socket.receive_buffer[i]);
    }
    printf("\n");
  }
  return NULL;
}
