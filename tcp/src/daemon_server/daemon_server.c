#include "../config.h"
#include "../tcp_stack.h"
#include <errno.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

void daemon_server(tcp_stack *stack) {
  int fd = socket(PF_LOCAL, SOCK_STREAM, 0);
  if (fd == -1) {
    fprintf(stderr, "Failed to create daemon server socket: %s\n", strerror(errno));
    exit(1);
  }

  struct sockaddr_un socket_address = (struct sockaddr_un){
      .sun_family = AF_LOCAL,
      .sun_path = DAEMON_SOCKET_LOCATION,
  };

  int bind_result = bind(fd, &socket_address, sizeof(struct sockaddr_un));
  // TODO...

  while (!atomic_load(stack->destroyed)) {
  }
}
